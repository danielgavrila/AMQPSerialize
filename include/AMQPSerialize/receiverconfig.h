#ifndef RECEIVERCONFIG_H
#define RECEIVERCONFIG_H
#include <string>
#include <sstream>
#include <proton/source_options.hpp>
namespace serializeAMQP{
struct receiverConfig
{
    std::string url;
    std::string address;
    proton::source_options opts;
    std::string getAddrUrl() const
    {
        return url+"/"+address;
    }

};

struct connectionConfig{
    // qpid-config queues -b guest/guest@broker-host:10000
    //auto url="amqp://127.0.0.1:5672";
    explicit connectionConfig( const std::string & userName, const std::string &password,const std::string &hostName,int port):
        userName(userName),password(password),hostName(hostName), port(port)
    {

    }

    connectionConfig()= delete;
    std::string getConnectionString() const
    {
        std::stringstream ss;
        ss << port;
        auto str=hostName+":"+ss.str();
        if(userName.size()>0)
        {
            auto tmp=userName+"/"+password;
            str=tmp+"@"+str;
        }
        return str;
    }
private:
    std::string hostName,  userName, password;
    int port;

};
}
#endif // RECEIVERCONFIG_H
