#ifndef STRUCTSDSL_H
#define STRUCTSDSL_H



#include "AMQPSerialize/serializeamqp.h"

/*
1. Define the structure MyStruct
2. Define the structute to be sent to AMQP , AMQPMyStruct derived from AMQPStructs
          <MyStruct,one identifier din enum TypesAMQP>;
3. Add the structure in the VariantStruct (must be in bijection with the enum TypesAMQP )
*/

// variable concept
template <class T, class U>
concept bool Derived = std::is_base_of_v<U, T>;

// function concept
template <class T>
concept bool EqualityComparable() {
    return requires(T a, T b) { {a == b} -> bool; {a != b} -> bool; };
}

template <class T>
concept bool Integral = std::is_integral_v<T>;
template <class T>
concept bool SignedIntegral = Integral<T> && std::is_signed_v<T>;
template <class T>
concept bool UnsignedIntegral = Integral<T> && !SignedIntegral<T>;

//template<typename T>
//concept bool Addable = requires (T x) { x + x; }; // requires-expression

template<typename T>
concept bool Addable =
        requires (T a, T b) {
        a + b; // "the expression a+b is a valid expression that will compile"
        };



template<typename T> requires Addable<T> // requires-clause, not requires-expression
T add(T a, T b) { return a + b; }

namespace Queue1{

constexpr int SIZE_STR=10;
struct foo{
    double z;
    int16_t s;
    std::string str;
    std::string str1;
};

struct TestStruct{
    int32_t i;
    double d;
    int8_t c;
    foo f;

};
inline bool operator==(const TestStruct& a,const TestStruct&b)
{
    return a.i==b.i ;
}

using VctTestStruct=std::vector<TestStruct>;

struct Point{
    double x,y,z;
};

constexpr bool operator==(const Point &p1,const Point &p2)
{
    return std::tie(p1.x,p1.y,p1.z)==std::tie(p2.x,p2.y,p2.z);
}

using VctPoint=std::vector<Point>;

struct Profile
{
    std::vector<Point>  arr;
};

inline bool operator==(const Profile &p1,const Profile &p2)
{
    bool b=true;
    for (auto i=0u; i< p1.arr.size();i++)
    {
        b=p1.arr[i]==p2.arr[i];
        if(false==b)
            break;
    }
    return b;
}

enum class StatusSensor:int32_t
{
    OK,
    NotAvailable,
    Busy
};

struct Temperature
{
    double qty;
    std::string unit;
    StatusSensor status;
};

inline bool operator==(const Temperature &p1,const Temperature &p2)
{
    return p1.qty==p2.qty && p1.unit==p2.unit && p1.status == p2.status;
}

using AMQPTestStruct=AMQPStructs<TestStruct,TypesAMQP::One>;
using AMQPVctTestStruct=AMQPStructs<VctTestStruct,TypesAMQP::Two>;
using AMQPPoint=AMQPStructs<Point,TypesAMQP::Three>;
using AMQPVctPoint=AMQPStructs<VctPoint,TypesAMQP::Four>;
using AMQPProfile=AMQPStructs<Profile,TypesAMQP::Five>;
using AMQPTemperature=AMQPStructs<Temperature,TypesAMQP::Six>;

//the first is std::monostate ,to keep the same values as in enum
using VariantStruct=std::variant<
                std::monostate,
                TestStruct,
                VctTestStruct,
                Point,
                VctPoint,
                Profile,
                Temperature
>;


template <typename TVctAMQP>
VariantStruct fromAMQP(const TVctAMQP &valVect )
{
    return  serializeAMQP::fromAMQPImpl<VariantStruct,TVctAMQP>(valVect );
}

VariantStruct  fromAMQP(const proton::message &m )
{
    return  serializeAMQP::fromAMQPImpl<VariantStruct>(m );
}


}//end  namespace

namespace serializeAMQP{
namespace detail{


    template<> struct isEnumAMQPType<Queue1::StatusSensor> : public std::true_type {};
    inline void getVariantImpl(const qpid::types::Variant &val,Queue1::StatusSensor &res)
    {
        res= static_cast<Queue1::StatusSensor>(val.asInt32());
    }


}
}

#endif // STRUCTSDSL_H