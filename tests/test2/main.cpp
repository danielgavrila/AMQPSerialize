#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "AMQPSerialize/serializeamqp.h"
#include "structures/structqueue2.h"



using namespace Queue2;

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

    }


    SECTION("TestWind")
    {
        auto ts=Wind{2.4,283.14};
        auto v=AMQPWind{ts};
        auto vctProtVal=serializeAMQP::toProton(v);
        auto var= Queue2::fromAMQP(vctProtVal);
        REQUIRE(static_cast<size_t>(TypesAMQP::One)==var.index());
        auto ts2=std::get<Queue2::Wind>(var);
        REQUIRE(ts==ts2);
    }

    SECTION("Parameters")
    {
        auto ts=Parameters{2,283,Ident::unu};
        auto v=AMQPParameters{ts};
        auto vctProtVal=serializeAMQP::toProton(v);
        auto var= Queue2::fromAMQP(vctProtVal);
        REQUIRE(static_cast<size_t>(TypesAMQP::Two)==var.index());
        auto ts2=std::get<Queue2::Parameters>(var);
        REQUIRE(ts==ts2);
    }


}

}
