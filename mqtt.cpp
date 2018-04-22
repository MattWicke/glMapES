#include "mqtt.h"
namespace mq
{
struct mosquitto *mosquitto_ptr = NULL;
}
bool synced = false;

void mq::message_callback(
        mosquitto *mosq_ptr
        ,void *userdata
        ,const mosquitto_message *message
        )
{
    if(message->payloadlen)
    {
        std::cout << message->topic << " " << message->payload << std::endl;
    }
    if(strcmp((char*)message->payload, "go") == 0)
        synced = true;
}

void mq::connect_callback(
        mosquitto *mosq_ptr
        , void *userdata
        ,int result
        )
{
    int i;
    if(!result)
    {
        std::cout << "connected to mosquitto" << std::endl;
    }
}

void mq::init(
        std::string hostName
        )
{
    int clientId = 3;
    char clientid[30];
    gethostname(clientid, 30);

    mosquitto_ptr = mosquitto_new(
            clientid
            ,true
            ,NULL
            );
    mosquitto_connect_callback_set(mosquitto_ptr, connect_callback);
    mosquitto_message_callback_set(mosquitto_ptr, message_callback);
    
    if(mosquitto_connect(
                mosquitto_ptr
                ,hostName.c_str()
                ,1883
                ,60
                ))
    {
        std::cout << "failed to connect to mosquitto" << std::endl;
    }

    if(mosquitto_subscribe(
                mosquitto_ptr
                ,NULL
                ,"cube"
                ,0
                ))
    {
        std::cout << "failed to subscribe to cube channel" << std::endl;
    }

    mosquitto_loop_start(mosquitto_ptr);
}
