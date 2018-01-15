#include "AMQPSerialize/serializeamqp.h"
#include "AMQPSerialize/receiverMTFlow.h"
#include "structures/structqueue2.h"



using namespace Queue2;
using namespace qpid::messaging;
using namespace serializeAMQP;


int main(int argc, const char **argv)
{
    // Run the proton container
    proton::container container;

    auto container_thread = std::thread([&]() { container.run(); });


    auto url=connectionConfig("","","127.0.0.1",5672).getConnectionString();
    auto address=std::string("CAPPI");//examples  StatusOverview

    receiverConfig recConfig;
    recConfig.url=url;
    recConfig.address=address;

    set_filter(recConfig.opts, "pdType = 'CAPPI'");


    receiver recv(container, recConfig,[](const proton::message &m)
    {
        std::cout <<  " received \"" << m.body() << '"' << std::endl;
    });

    auto thread=std::thread([&]() { receive_thread_forever(recv); });
    thread.join();
    recv.close();

    container_thread.join();
}

