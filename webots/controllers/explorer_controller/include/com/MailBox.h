//
// Created by luc on 31/05/19.
//

#ifndef EXPLORER_MAILBOX_H
#define EXPLORER_MAILBOX_H

#include <mutex>
#include <queue>
#include "AirplugMessage.h"

class MailBox {
    std::mutex mutex;
    std::queue<AirplugMessage> queue;
public:
    MailBox() = default;

    AirplugMessage pop() {
        AirplugMessage msg;
        mutex.lock();
        msg = queue.front();
        queue.pop();
        mutex.unlock();
        return msg;
    }

    void push(AirplugMessage msg) {
        mutex.lock();
        queue.push(msg);
        mutex.unlock();
    }

    const int size() {
        int size;
        mutex.lock();
        size = queue.size();
        mutex.unlock();
       return size;
    }
};


#endif //EXPLORER_MAILBOX_H
