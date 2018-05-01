#pragma once
#include <mosquitto.h>
#include <string>
#include <iostream>
#include <cstddef>
#include <string.h>
#include <unistd.h>
#include <chrono>

namespace mq
{
void message_callback(
        mosquitto *mosq_ptr
        ,void *userdata
        ,const mosquitto_message *message
        );

void connect_callback(
        mosquitto *mosq_ptr
        , void *userdata
        ,int result
        );

void init(
        std::string hostName
        );
void send(
        std::string m_message
        );

}//end mqtt namespace
