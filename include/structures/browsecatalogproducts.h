#ifndef BROWSECATALOGPRODUCTS_H
#define BROWSECATALOGPRODUCTS_H
#include "AMQPSerialize/receiverMTFlow.h"
#include "AMQPSerialize/receiverconfig.h"
#include "catalogproducts.h"

inline void BrowseCatalogProducts( proton::container &container,
                                   const serializeAMQP::connectionConfig &connCfg,
                                   VctDescProduct &res,
                                   bool bBrowse)
{


    serializeAMQP::receiverConfig recConfig;
    recConfig.url=connCfg.getConnectionString();
    recConfig.address="CatalogProducts";
    if(bBrowse)
        recConfig.opts.distribution_mode(proton::source::COPY);

    serializeAMQP::receiver recv(container, recConfig,[&res](const proton::message &m)->int
    {
        std::cout <<  " brwCatProd \"" << m.body() << '"' << std::endl;
        auto var=QueueCatalogProducts::fromAMQP(m);
        assert(static_cast<size_t>(serializeAMQP::TypesAMQP::One)==var.index());
        auto ts2=std::get<QueueCatalogProducts::CatalogProducts>(var);
        if(ts2==LastElement())
            return 1;
        else
        {
            res.emplace_back(ts2);
        }
        return 0;
    });

    auto thread=std::thread([&]() { receive_thread_forever(recv); });
    thread.join();
    std::cout << "end BrowseCatalogProducts\n"<< std::endl;
    recv.close();


}

#endif // BROWSECATALOGPRODUCTS_H
