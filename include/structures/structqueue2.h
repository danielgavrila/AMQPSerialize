#ifndef STRUCTQUEUE2_H
#define STRUCTQUEUE2_H

#include "AMQPSerialize/serializeamqp.h"

/*
1. Define the structure MyStruct
2. Define the structute to be sent to AMQP , AMQPMyStruct derived from AMQPStructs
          <MyStruct,one identifier din enum TypesAMQP>;
3. Add the structure in the VariantStruct (must be in bijection with the enum TypesAMQP )
*/


namespace Queue2
{
struct Wind{
    double speed;
    double dir;
};

inline bool operator==(const Wind &p1,const Wind &p2)
{
    return p1.speed==p2.speed && p1.dir==p2.dir;
}

enum  class Ident:int32_t
{
    zero,
    unu,
    doi
};
struct Parameters{
    int id;
    short type;
    Ident ident;
};


inline bool operator==(const Parameters &p1,const Parameters &p2)
{
    return p1.id==p2.id && p1.type==p2.type;
}

struct CAPPI
{
   Parameters params;
   proton::binary blob;
};

using AMQPWind=AMQPStructs<Wind,TypesAMQP::One>;
using AMQPParameters=AMQPStructs<Parameters,TypesAMQP::Two>;
using AMQPCAPPI=AMQPStructs<CAPPI,TypesAMQP::Three>;

using VariantStruct=std::variant<
                std::monostate,
                Wind,
                Parameters,
                AMQPCAPPI>;

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

namespace serializeAMQP{
namespace detail{


    template<> struct isEnumAMQPType<Queue2::Ident> : public std::true_type {};
    inline void getVariantImpl(const qpid::types::Variant &val,Queue2::Ident &res)
    {
        res= static_cast<Queue2::Ident>(val.asInt32());
    }


}
}


#endif // STRUCTQUEUE2_H
