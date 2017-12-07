# AMQPSerialize
C++17 header only library that serialize/deserialze a type T that must be constexpr aggregate initializable to AMQP  

Dependency : The library is using the excelent library magic_get from Antony Polukhin
https://github.com/apolukhin/magic_get and the Proton and Qpid libraries from Apache

The compiler that I used is gcc version 7.2 with the flag -fconcepts

To build the tests you have to use cmake version 3.10

cd build
mkdir build
cmake .. -DProton_DIR=/your-path-to-Proton/lib64/cmake/Proton 
         -DProtonCpp_DIR=/your-path-to-ProtonCpp/lib64/cmake/ProtonCpp 
         -DQpid_DIR=/your-path-to-Qpid/lib64/cmake/Qpid

Usage:You define under the same namespace all the POD structures that you want to serialize/deserialize.
Let say that for the queue Queue1 you will want send/receive the structure Point

	namespace Queue1
	{
	struct Point{
	    double x,y,z;
	};
	//associate to the struct one identifier  from the enum TypesAMQP
	using AMQPPoint=AMQPStructs<Point,TypesAMQP::One>;

	//the first is std::monostate ,to keep the same values as in enum TypesAMQP
	using VariantStruct=std::variant<std::monostate,Point>;

	//define the wrapers 
	template <typename TVctAMQP>
	VariantStruct fromAMQP(const TVctAMQP &valVect )
	{
	    return  serializeAMQP::fromAMQPImpl<VariantStruct,TVctAMQP>(valVect );
	}

	VariantStruct  fromAMQP(const proton::message &m )
	{
	    return  serializeAMQP::fromAMQPImpl<VariantStruct>(m );
	}


	}


To test the new structure


    SECTION("PointTest")
    {
    auto p1 =Point {1.1,2.1,3.1};
    auto listVariants1=serializeAMQP::toProton(AMQPPoint{p1});
    auto listVariants2=serializeAMQP::toQpid(AMQPPoint{p1});
    auto var1= Queue1::fromAMQP(listVariants1);
    REQUIRE(static_cast<size_t>(TypesAMQP::One)==var1.index());
    auto var2= Queue1::fromAMQP(listVariants2);
    
    REQUIRE(static_cast<size_t>(TypesAMQP::One)==var2.index());

    auto p2=std::get<Queue1::Point>(var1);
    REQUIRE(p1==p2);

    auto p3=std::get<Queue1::Point>(var2);
    REQUIRE(p1==p3);

    }



