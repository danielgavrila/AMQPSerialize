#ifndef CATALOGPRODUCTS_H
#define CATALOGPRODUCTS_H
#include "descprod.h"
#include "AMQPSerialize/serializeamqp.h"

/*
1. Define the structure MyStruct
2. Define the structute to be sent to AMQP , AMQPMyStruct derived from AMQPStructs
          <MyStruct,one identifier din enum TypesAMQP>;
3. Add the structure in the VariantStruct (must be in bijection with the enum TypesAMQP )
*/


namespace QueueCatalogProducts
{
using namespace serializeAMQP;

using CatalogProducts=DescriptionProduct;


using AMQPCatalogProducts=AMQPStructs<CatalogProducts,TypesAMQP::One>;
inline std::string toHashId(const CatalogProducts& obj)
{
    static constexpr char NgsKeySeparator='|';


    std::string s = obj.sensorId + NgsKeySeparator
      + obj.pdType+ NgsKeySeparator
      + obj.dataType + NgsKeySeparator
      + obj.pdfName;

    return s;
}
using VariantStruct=std::variant<
                std::monostate,
                CatalogProducts
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
using VctAMQPCatalogProducts=std::vector<AMQPCatalogProducts>;

}

#endif // CATALOGPRODUCTS_H
