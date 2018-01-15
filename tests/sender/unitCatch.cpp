#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "AMQPSerialize/serializeamqp.h"
#include "AMQPSerialize/senderMTFlow.h"
#include "structures/structqueue2.h"
#include "structures/catalogproducts.h"
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





//int main()
TEST_CASE( "AMQPSending", "[AMQPSending]" )
{
auto catProd1=DescriptionProduct {"RADAR1","CAPPI","dBZ","10km"};
auto catProd2=DescriptionProduct {"RADAR1","CAPPI","dBZ","20km"};
auto catProd3=DescriptionProduct {"RADAR2","MCAPPI","V","40km"};
auto catProd4=DescriptionProduct {"RADAR2","MCAPPI","V","80km"};



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


// Run the proton container
proton::container container;
//    SECTION( "Misc1" )
//    {

//auto logQpid=std::string("qpid-config.log");
//std::string line;

//        bp::pstream p1;
//        bp::pstream p2;
//        bp::pstream is;
//        std::error_code ec;
//        //int result = bp::system("qpid-config");
//        auto c1=bp::child ("qpid-config exchanges ",ec,bp::std_out > p1);
//        REQUIRE(ec.value()==0);
//        std::cout << ec.message()<< std::endl;

//        c1.wait();

//        auto c2=bp::child ("grep examples ",ec,bp::std_in < p1, bp::std_out > p2);
//        REQUIRE(ec.value()==0);
//        c2.wait();

//        int count=bp::system("wc -l ",ec,bp::std_in < p2, bp::std_out > is);
//        REQUIRE(ec.value()==0);


//        std::getline(is, line);

//        count=std::strtol(line.c_str(),nullptr,10);
//        std::cout << "count:" <<count << std::endl;

//        int result=0;
//        if (1==count)
//        {
////        result  = bp::system("qpid-config del queue examples --force",ec, bp::std_out > logQpid);
////        REQUIRE(ec.value()==0);
////        REQUIRE(0==result);
//        }
//        else
//        {
//            result = bp::system("qpid-config add exchange topic examples ",ec, bp::std_out > logQpid);
//            REQUIRE(ec.value()==0);
//            REQUIRE(result==0);
//        }

//        auto container_thread = std::thread([&]() { container.run(); });
//        auto url="amqp://127.0.0.1:5672";
//        auto address="examples";
//        sender send(container, url, address);

//        auto protSender=send.getSender();
//        auto target=protSender.target();
//        auto source=protSender.source();
//        auto cap=source.capabilities();
//        //auto addr=source.address();
//        //std::cout << source.filters().size()<<" ,capabilities\n";

//        for (auto &c:cap)
//            std::cout << c << std::endl;



//        auto m2=serializeAMQP::toProtonMessage(cappimsg);


//        m2.subject("Daniel");
//        m2.properties().put("colour", "green");


//        //threadSend.join();

//        auto threadSend2=std::thread([&]() { send_thread(send, m2); });
//        threadSend2.join();
//        send.close();

//        container_thread.join();
//    }

    SECTION( "QMF" )
    {

        commandToBroker(connectionConfig("","","127.0.0.1",5672),"StatusOverview",
                        "{create: always, node:{ durable:true ,x-declare: "
                        "{arguments :{'qpid.last_value_queue_key':'id'}}"
                        "                      } "
                        "}");

        commandToBroker(connectionConfig("","","127.0.0.1",5672),"CatalogProducts",
                        "{create: always, node:{ durable:true ,x-declare: "
                        "{arguments :{'qpid.last_value_queue_key':'hashId'}}"
                        "                      } "
                        "}");

        createTopic(connectionConfig("","","127.0.0.1",5672),"examples","topic");
        createTopic(connectionConfig("","","127.0.0.1",5672),"CAPPI","topic");

    }

    SECTION ("CatalogProduct")
    {
        auto container_thread = std::thread([&]() { container.run(); });

        auto url=connectionConfig("","","127.0.0.1",5672).getConnectionString();
        auto address="CatalogProducts";
        sender send(container, url, address);
        sendCatalogProduct(send,msgCatProd1);
        sendCatalogProduct(send,msgCatProd2);
        sendCatalogProduct(send,msgCatProd3);
        sendCatalogProduct(send,msgCatProd4);

        send.close();
        container_thread.join();
    }


SECTION ("CAPPI")
{
    auto container_thread = std::thread([&]() { container.run(); });

    auto url=connectionConfig("","","127.0.0.1",5672).getConnectionString();
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

