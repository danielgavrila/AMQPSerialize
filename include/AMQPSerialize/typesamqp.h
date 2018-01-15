#ifndef TYPESAMQP_H
#define TYPESAMQP_H
#include<tuple>
#include<variant>
#include<vector>
#include<utility>
#include<type_traits>
namespace serializeAMQP{
enum class TypesAMQP:int32_t
{
    NotValid=0,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Eleven,
    Twelve,
    Thirteen,
    Fourteen,
    Fifteen,
    Sixteen,
    Seventeen,
    Eighteen,
    Nineteen,
    Twenty
};



template <typename T,TypesAMQP ID> struct AMQPStructs
{
    T aPOD;
    TypesAMQP tAMQP{ID};//the  last value is the typeId
};

}


#endif // TYPESAMQP_H
