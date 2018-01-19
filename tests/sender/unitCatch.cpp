#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <chrono>
#include <thread>

#include "catch.hpp"
#include "AMQPSerialize/serializeamqp.h"
#include "AMQPSerialize/senderMTFlow.h"
#include "structures/structqueue2.h"
#include "structures/catalogproducts.h"
#include "structures/browsecatalogproducts.h"
#include "AMQPSerialize/receiverconfig.h"
#include "AMQPSerialize/createtopic.h"


using namespace Queue2;
using namespace QueueCatalogProducts;
using namespace qpid::messaging;
using namespace serializeAMQP;



void sendCAPPI( sender &send,const AMQPCAPPI &cappimsg)
{
    auto m2=serializeAMQP::toProtonMessage(cappimsg);


    m2.properties().put("sensorId", cappimsg.aPOD.params.sensorId);
    m2.properties().put("pdType", cappimsg.aPOD.params.pdType);
    m2.properties().put("dataType", cappimsg.aPOD.params.dataType);
    m2.properties().put("pdfName", cappimsg.aPOD.params.pdfName);




    auto threadSend2=std::thread([&]() { send_thread(send, m2); });
    threadSend2.join();

}


void sendCatalogProduct( sender &send,const AMQPCatalogProducts &msg)
{
    auto m2=serializeAMQP::toProtonMessage(msg);
    auto id=toHashId(msg.aPOD);
    m2.properties().put("hashId", id);
    m2.durable(true);
    auto threadSend2=std::thread([&]() { send_thread(send, m2); });
    threadSend2.join();

}

void sendCatalogProduct( sender &send,const VctAMQPCatalogProducts &vct)
{
    std::vector<proton::message> vctMsg;
    for(auto &msg:vct)
    {
        auto m2=serializeAMQP::toProtonMessage(msg);
        auto id=toHashId(msg.aPOD);
        m2.properties().put("hashId", id);
        m2.durable(true);
        vctMsg.emplace_back(m2);
    }
    auto threadSend2=std::thread([&]() { send_thread(send, vctMsg); });
    threadSend2.join();

}




//int main()
TEST_CASE( "AMQPSending", "[AMQPSending]" )
{
    auto connCfg=connectionConfig("","","localhost",5672);

    auto catProd1=DescriptionProduct {"RADAR1","CAPPI","dBZ","10km"};
    auto catProd2=DescriptionProduct {"RADAR1","CAPPI","dBZ","20km"};
    auto catProd3=DescriptionProduct {"RADAR2","CAPPI","V","46m"};
    auto catProd4=DescriptionProduct {"RADAR2","CAPPI","V","86km"};



    proton::binary blob;
    constexpr size_t size=26;
    blob.resize(size);

    for(auto i=0u;i< size/2;i++)
        blob[i]='a'+i%26;

    auto cappi=CAPPI{catProd1,blob};
    auto cappimsg=AMQPCAPPI{cappi};

    auto cappi1=CAPPI{catProd2,blob};
    auto cappimsg1=AMQPCAPPI{cappi1};


    for(auto i=0u;i< size;i++)
        blob[i]='a'+i%26;

    auto mcappi=CAPPI{catProd3,blob};
    auto mcappimsg=AMQPCAPPI{mcappi};

    auto mcappi1=CAPPI{catProd4,blob};
    auto mcappimsg1=AMQPCAPPI{mcappi1};




    //initialite catalogProducts


    auto msgCatProd1=AMQPCatalogProducts{catProd1};
    auto msgCatProd2=AMQPCatalogProducts{catProd2};
    auto msgCatProd3=AMQPCatalogProducts{catProd3};
    auto msgCatProd4=AMQPCatalogProducts{catProd4};
    auto msgLast=AMQPCatalogProducts{LastElement()};

    VctAMQPCatalogProducts vctCatalogProducts=
    {msgCatProd1,
     msgCatProd2,
     msgCatProd3,
     msgCatProd4,
     msgLast};

    // Run the proton container
    proton::container container;

    SECTION( "QMF" )
    {

        commandToBroker(connCfg,"StatusOverview",
                        "{create: always, node:{ durable:true ,x-declare: "
                        "{arguments :{'qpid.last_value_queue_key':'id'}}"
                        "                      } "
                        "}");

        commandToBroker(connCfg,"CatalogProducts",
                        "{create: always, node:{ durable:true ,x-declare: "
                        "{arguments :{'qpid.last_value_queue_key':'hashId'}}"
                        "                      } "
                        "}");

        createTopic(connCfg,"examples","topic");
        createTopic(connCfg,"CAPPI","topic");


        commandToBroker(connCfg,"testFanout",
                        "{create: always, node:{ type: fanout } }");
    }

    SECTION ("CatalogProduct")
    {
        using namespace std::chrono_literals;
        auto container_thread = std::thread([&]() { container.run(); });

        auto url=connCfg.getConnectionString();
        auto address="CatalogProducts";

        sender send(container, url, address);

        sendCatalogProduct(send,msgLast);
        std::cout << "sent last" << std::endl;

        //std::this_thread::sleep_for(1s);
        send.close();
        container_thread.join();
    }

    SECTION ("DeleteCatalogProduct")
    {

        auto container_thread = std::thread([&]() { container.run(); });
        auto address="CatalogProducts";

        VctDescProduct vctdescProd;
        auto threadCleanCatProd=std::thread([&]() { BrowseCatalogProducts(container,connCfg,vctdescProd,false); });
        threadCleanCatProd.join();
        std::cout << "end cleaning: size " <<vctdescProd.size()<< std::endl;
        container_thread.join();
    }

    SECTION ("SendCatalogProduct")
    {

        auto container_thread = std::thread([&]() { container.run(); });

        auto url=connCfg.getConnectionString();
        auto address="CatalogProducts";

        sender send(container, url, address);
        std::cout << "begin send" << std::endl;
        //std::this_thread::sleep_for(1s);
        sendCatalogProduct(send,vctCatalogProducts);
        std::cout << "end send" << std::endl;


        send.close();
        container_thread.join();
    }


    SECTION ("CAPPI")
    {
        auto container_thread = std::thread([&]() { container.run(); });


        auto url=connCfg.getConnectionString();
        auto address="CAPPI";
        sender send(container, url, address);
        sendCAPPI(send,cappimsg);
        sendCAPPI(send,cappimsg1);
        sendCAPPI(send,mcappimsg);
        sendCAPPI(send,mcappimsg1);


        send.close();

        container_thread.join();


    }



}

