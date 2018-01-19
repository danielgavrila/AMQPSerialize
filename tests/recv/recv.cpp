#include "AMQPSerialize/serializeamqp.h"
#include "AMQPSerialize/receiverMTFlow.h"
#include "structures/structqueue2.h"
#include "structures/browsecatalogproducts.h"
#include "options.hpp"

using namespace Queue2;
using namespace qpid::messaging;
using namespace serializeAMQP;


int main(int argc, const char **argv)
{
    // Run the proton container
    proton::container container;
    int mode=0;
    example::options opts(argc, argv);
    opts.add_value(mode, 'm', "mode", "filter product", "filter");
    opts.parse();

    auto cfgConn=connectionConfig("","","127.0.0.1",7000);

    auto cfgConnBrowse=connectionConfig("","","127.0.0.1",5672);


    auto address=std::string("CAPPI");//examples  StatusOverview
    VctDescProduct vctdescProd;
    auto container_thread = std::thread([&]() { container.run(); });

    receiverConfig recConfig;
    recConfig.url=cfgConn.getConnectionString();
    recConfig.address=address;

    if(mode==0)
        set_filter(recConfig.opts, "dataType = 'V'");
    else
        set_filter(recConfig.opts, "dataType = 'dBZ'");

    receiver recv(container, recConfig,[](const proton::message &m)->int
    {
        std::cout <<  " received \"" << m.body() << '"' << std::endl;
        return 0;
    });

    auto threadRecv=std::thread([&]() { receive_thread_forever(recv); });







    auto threadBrowse=std::thread([&]() { BrowseCatalogProducts(container,cfgConnBrowse,vctdescProd,true); });
    threadBrowse.join();
    std::cout<<"Catalog Products"<<std::endl;
    for(auto &a:vctdescProd)
    {
        std::cout<<QueueCatalogProducts::toHashId(a)<<std::endl;
    }


    threadRecv.join();

    recv.close();

    container_thread.join();
}

