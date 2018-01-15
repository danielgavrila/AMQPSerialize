/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

//
// C++11 or greater
//
// A multi-threaded client that sends and receives messages from multiple AMQP
// addresses.
//
// Demonstrates how to:
//
// - implement proton handlers that interact with user threads safely
// - block sender threads to respect AMQP flow control
// - use AMQP flow control to limit message buffering for receivers threads
//
// We define sender and receiver classes with simple, thread-safe blocking
// send() and receive() functions.
//
// These classes are also privately proton::message_handler instances. They use
// the thread-safe proton::work_queue and standard C++ synchronization (std::mutex
// etc.) to pass messages between user and proton::container threads.
//
// NOTE: no proper error handling

#ifndef RECEIVERMT_FLOW_H_
#define RECEIVERMT_FLOW_H_


#include <proton/connection.hpp>
#include <proton/connection_options.hpp>
#include <proton/container.hpp>
#include <proton/message.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/receiver.hpp>
#include <proton/receiver_options.hpp>
#include <proton/sender.hpp>
#include <proton/work_queue.hpp>
#include <proton/source_options.hpp>

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include "receiverconfig.h"


namespace serializeAMQP{
using  ContinuationReceive=std::function<void(const proton::message &m)>;

inline void set_filter(proton::source_options &opts, const std::string& selector_str) {
    proton::source::filter_map map;
    proton::symbol filter_key("selector");
    proton::value filter_value;
    // The value is a specific AMQP "described type": binary string with symbolic descriptor
    proton::codec::encoder enc(filter_value);
    enc << proton::codec::start::described()
        << proton::symbol("apache.org:selector-filter:string")
        << selector_str
        << proton::codec::finish();
    // In our case, the map has this one element
    map.put(filter_key, filter_value);
    opts.filters(map);
}


// Lock output from threads to avoid scramblin

// A thread safe receiving connection that blocks receiving threads when there
// are no messages available, and maintains a bounded buffer of incoming
// messages by issuing AMQP credit only when there is space in the buffer.
class receiver : private proton::messaging_handler {
    static const size_t MAX_BUFFER = 100; // Max number of buffered messages

    // Used in proton threads only
    proton::receiver receiver_;

    // Used in proton and user threads, protected by lock_
    std::mutex lock_;
    proton::work_queue* work_queue_;
    std::queue<proton::message> buffer_; // Messages not yet returned by receive()
    std::condition_variable can_receive_; // Notify receivers of messages
    receiverConfig receiverConfig_;
    ContinuationReceive continuationReceive_;
    bool bDone_;
  public:

    // Connect to url
    receiver(proton::container& cont,  receiverConfig &recConfig,const ContinuationReceive &continuationReceive)
        : work_queue_(),receiverConfig_(recConfig),continuationReceive_(continuationReceive),bDone_(false)
    {
        // NOTE:credit_window(0) disables automatic flow control.
        // We will use flow control to match AMQP credit to buffer capacity.
        cont.open_receiver(recConfig.getAddrUrl(), proton::receiver_options().source(recConfig.opts).credit_window(0),
                           proton::connection_options().handler(*this));
    }

    // Thread safe receive
    proton::message receive() {
        std::unique_lock<std::mutex> l(lock_);
        // Wait for buffered messages
        while (!work_queue_ || buffer_.empty())
            can_receive_.wait(l);
        proton::message m = std::move(buffer_.front());
        buffer_.pop();
        // Add a lambda to the work queue to call receive_done().
        // This will tell the handler to add more credit.
        work_queue_->add([=]() { this->receive_done(); });
        return m;
    }

    void close() {
        std::lock_guard<std::mutex> l(lock_);
        if (work_queue_) work_queue_->add([this]() { this->receiver_.connection().close(); });
    }

    bool bDone() const
    {
    return bDone_;
    }

    void setDone(bool bDone)
    {
    bDone_ = bDone;
    }



    ContinuationReceive continuationReceive() const
    {
    return continuationReceive_;
    }


private:
    // ==== The following are called by proton threads only.


    void on_receiver_open(proton::receiver& r) override {
        receiver_ = r;
        std::lock_guard<std::mutex> l(lock_);
        work_queue_ = &receiver_.work_queue();
        receiver_.add_credit(MAX_BUFFER); // Buffer is empty, initial credit is the limit

    }

    void on_message(proton::delivery &d, proton::message &m) override {
        // Proton automatically reduces credit by 1 before calling on_message
        std::lock_guard<std::mutex> l(lock_);
        buffer_.push(m);
        can_receive_.notify_all();
    }

    // called via work_queue
    void receive_done() {
        // Add 1 credit, a receiver has taken a message out of the buffer.
        receiver_.add_credit(1);
    }

    void on_error(const proton::error_condition& e) override {
        std::cerr << "unexpected error: " << e << std::endl;

    }
};


// Receive messages till atomic remaining count is 0.
// remaining is shared among all receiving threads
inline void receive_thread_forever(receiver& r)
{
    std::cout << "start thread"<< std::endl;
    while(!r.bDone())
    {
        auto m = r.receive();    
        r.continuationReceive()(m);
    }
}

inline void receive_thread_once(receiver& r)
{
    std::cout << "start thread"<< std::endl;
    auto m = r.receive();
    r.continuationReceive()(m);
}


}
#endif



