#include "mqtt.h"
namespace mq
{
struct mosquitto *mosquitto_ptr = NULL;
}
bool master_waiting = false;
std::chrono::system_clock::time_point trigger_time =
    std::chrono::system_clock::time_point::max();
int done_count = 0;

void mq::message_callback(
        mosquitto *mosq_ptr
        ,void *userdata
        ,const mosquitto_message *message
        )
{
    char *payload = (char*)message->payload;
    if(message->payloadlen)
    {
        std::cout << message->topic << " " << message->payload << std::endl;
    }
    if(strcmp((char*)message->payload, "go") == 0)
        master_waiting = true;
    if(payload[0] == 'o')
    {
        long long in_time = atoll(payload + 1);
        std::chrono::milliseconds in_dur(in_time);
        trigger_time = std::chrono::system_clock::time_point(in_dur);
    }
    if(payload[0] == 'd')
    {
        done_count++;
        if(done_count >= 2)
        {
            std::stringstream time_string;
            time_string << "o";
            std::chrono::milliseconds offset(1300);

            time_string << std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch() + offset
                    ).count();
            mq::send(time_string.str());

            done_count = 0;
        }
    }
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

void mq::send(
        std::string m_message
        )
{
    mosquitto_publish(
            mosquitto_ptr
            ,NULL //message id
            ,"cube"
            ,m_message.size()
            ,m_message.c_str()
            ,0 //qos
            ,false //retain
            );
}
