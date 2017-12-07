#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "AMQPSerialize/serializeamqp.h"
#include "structures/structsdsl.h"



using namespace Queue1;


namespace{

struct TestVct{
    int32_t i;
    float f;
    std::vector<double> vct;
    int8_t s;

};
inline bool operator==(const TestVct&a,const TestVct &b)
{
    return (a.i==b.i) && (a.s==b.s) && (a.vct==b.vct);

}


struct ABC { uint32_t i;
             uint8_t s;
             std::array<int,3> arr;
             std::string str;
             TypesAMQP typesAMQP=TypesAMQP::One;
           };

bool operator==(const ABC&a,const ABC &b)
{
    return a.i == b.i && a.s == b.s;
}


TEST_CASE( "AMQPSerialize ", "[AMQPSerialize]" )
{

    SECTION( "Misc1" )
    {
    qpid::types::Variant valc('f');
    auto c=serializeAMQP::get<char>(valc);
    REQUIRE(c=='f');


    std::list<int> a={1,2,3,4,5,6,7,8,9};
    auto it=a.begin();
    std::advance(it,2);
    REQUIRE(*it==3);

    qpid::types::Variant val(12.5f);
    auto dbl=serializeAMQP::get<double>(val);
    REQUIRE(dbl==12.5);

    qpid::types::Variant val1(12u);
    auto ul=serializeAMQP::get<uint64_t>(val1);
    REQUIRE(ul==12);


    auto ts=TestStruct{2,3.14,'c',{4.3,45,"hello","world"}};

    AMQPTestStruct v{ts};


    auto listVariants=serializeAMQP::toQpid(v);
    qpid::messaging::Message request;
    request.setContentObject(listVariants);


    auto var= Queue1::fromAMQP(listVariants);
    REQUIRE(static_cast<size_t>(TypesAMQP::One)==var.index());
    auto ts2=std::get<TestStruct>(var);

    REQUIRE(ts==ts2);


    }

    SECTION("PointTest")
    {
    auto p1 =Point {1.1,2.1,3.1};



    auto listVariants2=serializeAMQP::toQpid(AMQPPoint{p1});
    auto var2= Queue1::fromAMQP(listVariants2);
    REQUIRE(static_cast<size_t>(TypesAMQP::Three)==var2.index());
    auto p2=std::get<Queue1::Point>(var2);
    REQUIRE(p1==p2);
    }

    SECTION("ProfileTest")
    {
    auto p1 =Point {1.1,1.2,1.3};
    auto p2 =Point {2.1,2.2,2.3};
    auto p3 =Point {3.1,3.2,3.3};

    auto profile=Profile{{p1,p2,p3}};
    auto listVariants3=serializeAMQP::toProton(AMQPProfile{profile});


    auto var3= Queue1::fromAMQP(listVariants3);
    REQUIRE(static_cast<size_t>(TypesAMQP::Five)==var3.index());
    auto profile2=std::get<Profile>(var3);

    REQUIRE(profile==profile2);

    }



    SECTION( "VctPoints")
    {
    auto p1 =Point {1.1,1.2,1.3};
    auto p2 =Point {2.1,2.2,2.3};
    auto p3 =Point {3.1,3.2,3.3};

    VctPoint vctPoint{p1,p2,p3};

    auto listVariants4=serializeAMQP::toQpid(AMQPVctPoint{vctPoint});
    auto var4= Queue1::fromAMQP(listVariants4);
    REQUIRE(static_cast<size_t>(TypesAMQP::Four)==var4.index());
    auto vctPoint2=std::get<Queue1::VctPoint>(var4);
    REQUIRE(vctPoint2==vctPoint);
    }



    SECTION("VctTest")
    {

    TestVct testVct1={2,1.2,{4.5,6.7,8.9},99};
    auto listVariants5=serializeAMQP::toProton(testVct1);

    auto var5= serializeAMQP::detail::fromPODAMQPImpl
            <TestVct,serializeAMQP::TVctProtonScalar>(listVariants5);

    REQUIRE(testVct1==var5);

    }

    SECTION("Unit1_Misc")
    {
    using namespace boost::pfr::ops;
    using TTuple=std::tuple <std::vector<int>,double,char>;


    ABC abc {45, 46,{87,88,89},"dan"};
    using TypeField =  boost::pfr::tuple_element_t<0, ABC>;
    //std::cout << "TypeField: " << typeid(TypeField).name() << '\n';
    static_assert(std::is_same_v<TypeField,uint32_t>,"the same");
    REQUIRE(serializeAMQP::detail::isAMQPType<uint32_t>::value==1);
    auto serABC=serializeAMQP::toProton(abc);



    auto abc2=serializeAMQP::detail::fromPODAMQPImpl<ABC,serializeAMQP::TVctProtonScalar>(serABC);
    REQUIRE(abc==abc2);

    }
    SECTION("TestStruct")
    {

        auto ts=TestStruct{2,3.14,'c',{4.3,45,"hello","world"}};
        AMQPTestStruct v{ts};
        auto vctProtVal=serializeAMQP::toProton(v);
        auto var= Queue1::fromAMQP(vctProtVal);
        REQUIRE(static_cast<size_t>(TypesAMQP::One)==var.index());
        auto ts2=std::get<Queue1::TestStruct>(var);

        REQUIRE(ts==ts2);

    }

    SECTION("TestTemperature")
    {

        auto ts=Temperature{283.14,"Kelvin",StatusSensor::Busy};
        AMQPTemperature v{ts};
        auto vctProtVal=serializeAMQP::toProton(v);
        auto var= Queue1::fromAMQP(vctProtVal);
        REQUIRE(static_cast<size_t>(TypesAMQP::Six)==var.index());
        auto ts2=std::get<Queue1::Temperature>(var);

        REQUIRE(ts==ts2);

    }



}

}
