#ifndef SERIALIZEAMQP_H
#define SERIALIZEAMQP_H

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <list>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>
#include <variant>
#include <type_traits>
#include <experimental/type_traits>


#include <proton/message.hpp>
#include <proton/codec/encoder.hpp>
#include <proton/codec/decoder.hpp>
#include <proton/codec/list.hpp>
#include <proton/codec/map.hpp>
#include <proton/codec/vector.hpp>
#include <proton/internal/type_traits.hpp>

#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>


#include <boost/pfr.hpp>
#include "boost/pfr/precise.hpp"

#include "typesamqp.h"
#include "applyindextuple.h"


namespace serializeAMQP{



using TVctProtonScalar=std::vector<proton::scalar>;
using TListQpidVariant=std::list<qpid::types::Variant>;


template<typename>
struct isQpidVct
        : public std::false_type { };

template<>
struct isQpidVct<TVctProtonScalar>
        : public std::true_type { };

template<>
struct isQpidVct<TListQpidVariant>
        : public std::true_type { };

template <typename T>
inline constexpr bool  isQpidVct_v=isQpidVct<T>::value;

template <typename T>
concept bool QpidVector=isQpidVct_v<T>;





namespace detail{

template <class T> struct isEnumAMQPType: public std::false_type{};
template<> struct isEnumAMQPType<TypesAMQP> : public std::true_type {};


template <class T>
constexpr bool isEnumAMQPType_v=isEnumAMQPType<T>::value;

template <class T> struct isAMQPType: public std::false_type{};
template<> struct isAMQPType<bool> : public std::true_type{};
template<> struct isAMQPType<uint8_t> : public std::true_type {};
template<> struct isAMQPType<int8_t> : public std::true_type {};
template<> struct isAMQPType<uint16_t> : public std::true_type {};
template<> struct isAMQPType<int16_t> : public std::true_type{};
template<> struct isAMQPType<uint32_t> : public std::true_type {};
template<> struct isAMQPType<int32_t> : public std::true_type{};
template<> struct isAMQPType<uint64_t> : public std::true_type {};
template<> struct isAMQPType<int64_t> : public std::true_type{};
template<> struct isAMQPType<wchar_t> : public std::true_type {};
template<> struct isAMQPType<float> : public std::true_type{};
template<> struct isAMQPType<double> : public std::true_type {};
template<> struct isAMQPType<std::string> : public std::true_type {};


//template<> struct isAMQPType<symbol> : public std::true_type<SYMBOL, symbol> {};
//template<> struct isAMQPType<binary> : public std::true_type<BINARY, binary> {};
//template<> struct isAMQPType<timestamp> : public std::true_type<TIMESTAMP, timestamp> {};
//template<> struct isAMQPType<decimal32> : public std::true_type<DECIMAL32, decimal32> {};
//template<> struct isAMQPType<decimal64> : public std::true_type<DECIMAL64, decimal64> {};
//template<> struct isAMQPType<decimal128> : public std::true_type<DECIMAL128, decimal128> {};
//template<> struct isAMQPType<uuid> : public std::true_type<UUID, uuid> {};
template<typename T>
constexpr bool isAMQPType_v=isAMQPType<T>::value;

template<typename>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T,A>> : std::true_type {};

template<typename T>
  constexpr bool is_std_vector_v=is_std_vector<T>::value;

template<class T>
using hasListType =typename T::List;

static_assert(std::experimental::is_detected_v<hasListType, qpid::types::Variant>,
              "hasListType");

template <typename T> requires QpidVector<T>
inline constexpr bool isQpidImplementation=
        std::experimental::is_detected_v<hasListType, typename T::value_type>;//detect if is qpid::types::Variant


template <typename T>void getVariantImpl(const qpid::types::Variant &,T &)
{
    assert(false);
}

inline void getVariantImpl(const qpid::types::Variant &val,bool &res)
{
    res= val.asBool();
}

inline void getVariantImpl(const qpid::types::Variant &val,uint8_t &res)
{
    res= val.asUint8();
}

inline void getVariantImpl(const qpid::types::Variant &val,uint16_t &res)
{
    res= val.asUint16();
}

inline void getVariantImpl(const qpid::types::Variant &val,uint32_t &res)
{
    res= val.asUint32();
}

inline void getVariantImpl(const qpid::types::Variant &val,uint64_t &res)
{
    res= val.asUint64();
}

inline void getVariantImpl(const qpid::types::Variant &val,int8_t &res)
{
    res= val.asInt8();
}

inline void getVariantImpl(const qpid::types::Variant &val,char &res)
{
    assert(sizeof(char)==1);
    assert(true==std::is_signed_v<char>);
    res= val.asInt8();
}


inline void getVariantImpl(const qpid::types::Variant &val,int16_t &res)
{
    res= val.asInt16();
}

inline void getVariantImpl(const qpid::types::Variant &val,int32_t &res)
{
    res= val.asInt32();
}



inline void getVariantImpl(const qpid::types::Variant &val,int64_t &res)
{
    res= val.asInt64();
}


inline void getVariantImpl(const qpid::types::Variant &val,float &res)
{
    res= val.asFloat();
}


inline void getVariantImpl(const qpid::types::Variant &val,double &res)
{
    res= val.asDouble();
}

inline void getVariantImpl(const qpid::types::Variant &val,std::string &res)
{
    res= val.asString();
}

inline void getVariantImpl(const qpid::types::Variant &val,TypesAMQP &res)
{
    res= static_cast<TypesAMQP>(val.asInt32());
}


template <typename T>
T getVariant(const qpid::types::Variant &val)
{
    auto res=T();
    detail::getVariantImpl(val,res);
    return res;
}



//TypeField is from basic AMQPType
template <typename TypeField,typename TVctAMQP >requires QpidVector<TVctAMQP>
constexpr void addBasicType(const TypeField & fieldValue,TVctAMQP &res)
{
    if constexpr (isEnumAMQPType_v<TypeField>)
    {
        auto fieldValueInt=static_cast<int32_t>(fieldValue);
        res.emplace_back(fieldValueInt);
    }
    else
    {
        res.emplace_back(fieldValue);
    }
}

//T is aggregate initialisable structure
//TVctAMQP requires to be a vector of proton::scalar,qpid::types::Variant
template <int N,typename T,typename TVctAMQP > requires QpidVector<TVctAMQP>
void toAMQPImplRec(const T &val,TVctAMQP &res)
{
    if constexpr (N > 0)
    {
        constexpr auto indexTuple =  N-1;
        auto valueField =boost::pfr::get<indexTuple>(val);
        using TypeField =  boost::pfr::tuple_element_t<indexTuple, T>;

        if constexpr (isAMQPType_v<TypeField>|| isEnumAMQPType_v<TypeField>) //basic type
        {
            addBasicType<TypeField,TVctAMQP>(valueField,res);
        }
        else
        {
            if constexpr (is_std_vector_v<TypeField>)
            {
                using TypeElementArray=typename TypeField::value_type;
                auto sizeArray=valueField.size();
                constexpr auto M= boost::pfr::tuple_size_v<TypeElementArray>;

                res.emplace_back(sizeArray);//add  size of array
                for (auto k=sizeArray;k!=0;k--)//scroll over array and convert
                {
                    auto item=valueField[k-1];
                    toAMQPImplRec<M,TypeElementArray,TVctAMQP>(item,res);
                }
            }
            else
            {
                //aggregate structure
                constexpr auto M= boost::pfr::tuple_size_v<TypeField>;
                toAMQPImplRec<M,TypeField,TVctAMQP>(valueField,res);
            }
        }
        toAMQPImplRec<N-1,T,TVctAMQP>(val,res);//recursion
    }
}


//T is aggregate initialisable structure
//TVctAMQP requires to be a vector of proton::scalar,qpid::types::Variant
template <typename T,typename TVctAMQP > requires QpidVector<TVctAMQP>
constexpr TVctAMQP toAMQPImpl(const T &val)
{
    auto v=TVctAMQP();
    constexpr int N= boost::pfr::tuple_size_v<T>;

    toAMQPImplRec<N,T,TVctAMQP>(val,v);
    std::reverse(v.begin(),v.end());

    return v;
}



//use the convention from STL, a range is defined as [begin,end)
template <typename TVctAMQP > requires QpidVector<TVctAMQP>
constexpr TVctAMQP selectionVector(const TVctAMQP &val,const size_t &start,const size_t &end)
{
    auto sel= TVctAMQP();
    auto from=val.begin();
    std::advance(from,start);
    auto to=val.begin();
    std::advance(to,end);
    std::copy(from,to,std::back_inserter(sel));
    return sel;
}

template <typename TVctAMQP > requires QpidVector<TVctAMQP>
constexpr size_t getSizeVector(const TVctAMQP &val,const size_t &indexVector)
{
    size_t res=0;
    if constexpr (isQpidImplementation<TVctAMQP>)
    {

        auto it=val.begin();
        std::advance(it,indexVector);
        res = getVariant<size_t>(*it);//copy effective

    }
    else
    {
        res=proton::get<size_t>(val[indexVector]);
    }
    return res;
}

template <size_t indexTuple,typename T,typename TVctAMQP  ,typename TypeField> requires QpidVector<TVctAMQP>
void assignFromProton(const TVctAMQP &val,T&res,const size_t &indexVector)
{
    if constexpr (isEnumAMQPType_v<TypeField>)
    {
        auto tmp=proton::get<int32_t>(val[indexVector]);
        boost::pfr::get<indexTuple>(res)=static_cast<TypeField>(tmp);
    }
    else
    {
        boost::pfr::get<indexTuple>(res)=proton::get<TypeField>(val[indexVector]);
    }

}


template <int N,typename T,typename TVctAMQP  > requires QpidVector<TVctAMQP>
void fromPODToAMQPImplRec(const TVctAMQP &val,T&res, size_t &indexVector)
{
    if constexpr (N > 0)
    {
        constexpr auto indexTuple =  N-1;
        //auto fieldValue =boost::pfr::get<indexTuple>(val);

        using TypeField =  boost::pfr::tuple_element_t<indexTuple, T>;

        if constexpr (isAMQPType_v<TypeField> || isEnumAMQPType_v<TypeField>) //basic type
        {
            indexVector--;
            if constexpr (isQpidImplementation<TVctAMQP>)
            {

                auto it=val.begin();
                std::advance(it,indexVector);
                boost::pfr::get<indexTuple>(res) = getVariant<TypeField>(*it);//copy effective

            }
            else
            {
                assignFromProton<indexTuple,T,TVctAMQP,TypeField>(val,res,indexVector);
            }
        }
        else
        {//aggregate structure

            if constexpr (is_std_vector_v<TypeField>)
            {
                indexVector--;
                using TypeElementArray=typename TypeField::value_type;
                auto valueField =TypeField();
                auto sizeArray=getSizeVector(val,indexVector);
                constexpr auto N1= boost::pfr::tuple_size_v<TypeElementArray>;

                valueField.resize(sizeArray);
                indexVector=indexVector-sizeArray*N1;
                for (auto k=sizeArray;k!=0;k--)//scroll over array and convert
                {
                    auto ts = TypeElementArray();
                    auto sel= selectionVector(val,indexVector+(k-1)*N1,indexVector+k*N1);
                    auto indexSelectionVector=N1;
                    fromPODToAMQPImplRec<N1,TypeElementArray,TVctAMQP>(sel,ts,indexSelectionVector);
                    valueField[k-1]=ts;
                }
                boost::pfr::get<N-1>(res)=valueField;


            }
            else
            {
                constexpr auto M= boost::pfr::tuple_size_v<TypeField>;
                //copy to sel vector the  part of initial vector
                //that corespond to this aggregate structure
                auto sel=selectionVector(val,indexVector-M,indexVector);
                indexVector=indexVector-M;


                auto resTypeField= TypeField();

                auto length=M;
                fromPODToAMQPImplRec<M,TypeField,TVctAMQP>(sel,resTypeField,length);
                boost::pfr::get<indexTuple>(res)=resTypeField;
            }



        }

        fromPODToAMQPImplRec<N-1,T,TVctAMQP>(val,res,indexVector);
    }
}


//TVctAMQP requires to be a vector of proton::scalar,qpid::types::Variant
template <typename T,typename TVctAMQP > requires QpidVector<TVctAMQP>
T fromPODAMQPImpl(const TVctAMQP &val)
{

    if constexpr (is_std_vector_v<T>)
    {
        using TypeElement= typename T::value_type;
        auto vct =T();
        static constexpr size_t size=boost::pfr::tuple_size_v<TypeElement>;
        for(auto i=0u;i<val.size()-1;i+=size )//ignore the  last value which is the typeId
        {
            auto tmp=selectionVector(val,i,i+size);
            auto s=fromPODAMQPImpl<TypeElement,TVctAMQP>(tmp);
            vct.emplace_back(s);
        }
        return vct;

    }
    else
    {
        constexpr auto N= boost::pfr::tuple_size_v<T>;
        auto indexVector=val.size();

        auto ts = T();
        fromPODToAMQPImplRec<N,T,TVctAMQP>(val,ts,indexVector);
        return ts;
    }

}

//TVctAMQP requires to be a vector of proton::scalar,qpid::types::Variant
template <typename TVctAMQP > requires QpidVector<TVctAMQP>
TVctAMQP eraseLastValue(const TVctAMQP & valVect)
{
    auto  val=valVect;//copy the values and  erase the last value!
    auto it=val.begin();
    std::advance(it,val.size()-1);
    val.erase(it);
    return val;
}

template<typename TVctAMQP, typename ...T >
struct initFunctionsTuple
{
    static decltype(auto) doIt()
    {
        return std::make_tuple(serializeAMQP::detail::fromPODAMQPImpl<T,TVctAMQP>...);
    }
};


//T is a tuple of  POD
template<typename T,typename TVctAMQP> requires QpidVector<TVctAMQP>
using init_FN_tuple = decltype(
                      moveParamPacks<TVctAMQP,initFunctionsTuple,std::tuple>
                     (std::declval<T>()));



}//end detail



template <typename T > TVctProtonScalar toProton(const T &val)
{
    return detail::toAMQPImpl<T,TVctProtonScalar>(val);
}


template <typename T > TListQpidVariant toQpid(const T &val)
{
    return detail::toAMQPImpl<T,TListQpidVariant>(val);
}



//TVctAMQP requires to be a vector of proton::scalar,qpid::types::Variant
template <typename Result,typename TVctAMQP> requires QpidVector<TVctAMQP>
Result fromAMQPImpl(const TVctAMQP &valVect )
{
    auto res=Result();

    using AMQPTuple=
    to_tuple_from_variant_with_zero<Result>;

    //initialize tuple of functions serializeAMQP::detail::fromPODAMQPImpl
    auto tupleOfFunctions=detail::init_FN_tuple<AMQPTuple,TVctAMQP >::doIt();



    auto idTypeStruct=TypesAMQP::NotValid;

    auto lastValue=valVect.back();
    if constexpr (detail::isQpidImplementation<TVctAMQP>)
            idTypeStruct=static_cast<TypesAMQP>(detail::getVariant<int32_t>(lastValue));
    else
    idTypeStruct=static_cast<TypesAMQP>(proton::get<int32_t>(lastValue));


    auto  val=detail::eraseLastValue(valVect);

    auto k=static_cast<size_t>(idTypeStruct)-1;//TypesAMQP contains also the 0 state

    assert(k<std::tuple_size_v<decltype(tupleOfFunctions)>);
    applyIndex(tupleOfFunctions, k, [val=val,&res](auto p)
    {
        res=p(val);
    });


    return res;

}

template <typename Result>
Result fromAMQPImpl(const proton::message &m )
{

    auto valMsg=m.body().type();
    assert(proton::LIST==valMsg);
    TVctProtonScalar  vct;
    proton::get(m.body(), vct);
    return fromAMQPImpl<Result,TVctProtonScalar>(vct);
}

template <typename T>
T get(const qpid::types::Variant &val)
{
    return detail::getVariant<T>(val);
}

}

#endif // SERIALIZEAMQP_H
