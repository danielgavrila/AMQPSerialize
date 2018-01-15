#ifndef CREATETOPIC_H
#define CREATETOPIC_H
#include <qpid/messaging/Address.h>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>

#include "receiverconfig.h"
namespace serializeAMQP{
using QMFAddress= std::string;

void inline  commandToBroker(const connectionConfig & connCfg,const std::string &nameTopic,const QMFAddress &qmfAddress)
    {
        std::string broker =  connCfg.getConnectionString();
        std::string address = nameTopic+ "; "+qmfAddress;
        std::string connectionOptions =  "";

        auto connection=qpid::messaging::Connection (broker, connectionOptions);
        connection.open();
        connection.createSession().createSender(address);
        connection.close();

    }



void inline  createTopic(const connectionConfig & connCfg,const std::string &nameTopic,const std::string &typeTopic)
    {
        std::string broker =  connCfg.getConnectionString();
        std::string address = nameTopic+ "; {create: always, node:{ type: "+typeTopic+",durable:true } }";
        std::string connectionOptions =  "";

        auto connection=qpid::messaging::Connection (broker, connectionOptions);
        connection.open();
        connection.createSession().createSender(address);
        connection.close();

    }
}

#endif // CREATETOPIC_H
