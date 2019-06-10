//
// Created by luc on 21/05/19.
//

#include <com/RobAck.h>
#include "Net.h"

void Net::run() {
    while (run_) {
        // Handling incoming messages from monitoring app
        if (udpServer_.getNumberOfMessages() > 0) {
            AirplugMessage msg(udpServer_.popMessage());
            std::cout << "NET " << id_ << ": Received this message from UDP: " << msg.serialize() << std::endl;

            // Handling connection
            if (msg.getValue("typemsg") == "MAPCO") {
                monitors_.emplace(msg.getValue("ip") + ":" + msg.getValue("port"), new UDPClient(msg.getValue("ip"), std::stoi(msg.getValue("port"))));
                subscribers["EXP"]->push(msg);
                std::cout << "New monitor at: " << msg.getValue("ip") << ":" << msg.getValue("port") << std::endl;
            }

            // Handling disconnection
            else if (msg.getValue("typemsg") == "ENDCO") {
                monitors_.erase(msg.getValue("ip") + ":" + msg.getValue("port"));
            }

            // Handling robot order
            else if (msg.getValue("typemsg") == "ROBMSG"){
                msg.remove("typemsg");
                std::cout << id_ << " Passing to subscriber " << msg.serialize() << std::endl;
                subscribers[msg.getDestinationApp()]->push(msg);
            }
        }

        // Handling messages from local APPs
        while (lchMailBox_.size() > 0) {
            AirplugMessage msg = lchMailBox_.pop();
            // Logging
            if (msg.getEmissionApp() != "ROB" || RobAck(msg.getValue("roback")).getType() != RobAck::curr)
                std::cout << id_ << ": Received this message from LCH: " << msg.serialize() << std::endl;

            // Handling messages from monitoring app
            if (msg.getDestinationApp() == "MAP") {
                for (auto monitor : monitors_) {
                    monitor.second->sendMessage(msg.serialize());
                }
            }
            // Handling messages from local app
            else if (msg.getType() == AirplugMessage::local) {
                subscribers[msg.getDestinationApp()]->push(msg);
            }
            // Handling messages from air app
            else if (msg.getType() == AirplugMessage::air) {
                // Adding the message number before sending it
                msg.add("msgnbr", std::to_string(++lastMessageSentNumber_));
                airOutMailBox_->push(msg);
            }
        }

        // Handling messages from Air Apps
        while (airInMailBox_.size() > 0) {
            AirplugMessage msg = airInMailBox_.pop();

            string sender = msg.getValue("sender");
            string destination = msg.getValue("dest");
            int messageNumber = std::stoi(msg.getValue("msgnbr"));

            // Checking if the message should be considered
            if(sender != id_ && messageNumber > lastMessagesReceived_[sender]) {
                // Message was never received, so we treat it
                lastMessagesReceived_[sender] = messageNumber;

                // Logging
                std::cout << id_ << ": Received this message from AIR: " << msg.serialize() << std::endl;

                if(destination == BROADCAST) {
                    // The message is a broadcast : we have to transfer it & to treat it
                    airOutMailBox_->push(msg);
                    subscribers[msg.getDestinationApp()]->push(msg);
                } else {
                    // Checking if the message is for us
                    if(destination == id_ || destination == NEIGHBOURHOOD) {
                        // The message is for us, we give it to the destination app
                        subscribers[msg.getDestinationApp()]->push(msg);
                    } else {
                        // The message is not for us : we transfer it
                        airOutMailBox_->push(msg);
                    }

                }
            }
        }

        // Pause for 10 ms
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
    }
}
