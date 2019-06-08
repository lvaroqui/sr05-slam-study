//
// Created by luc on 21/05/19.
//

#include "Net.h"

void Net::run() {
    while (true) {
        if (udpServer_.getNumberOfMessages() > 0) {
            AirplugMessage msg(udpServer_.popMessage());
            std::cout << "NET " << id_ << ": Received this message from UDP: " << msg.serialize() << std::endl;
            // Handling incoming messages from monitoring app
            if (msg.getValue("typemsg") == "MAPCO") {
                monitors_.emplace(msg.getValue("ip") + ":" + msg.getValue("port"), new UDPClient(msg.getValue("ip"), std::stoi(msg.getValue("port"))));
                subscribers["EXP"]->push(msg);
                std::cout << "New monitor at: " << msg.getValue("ip") << ":" << msg.getValue("port") << std::endl;
            }
            // Handling disconnection
            else if (msg.getValue("typemsg") == "ENDCO") {
                monitors_.erase(msg.getValue("ip") + ":" + msg.getValue("port"));
            }
            else if (msg.getValue("typemsg") == "ROBMSG"){
                msg.remove("typemsg");
                std::cout << id_ << " Passing to subscriber " << msg.serialize() << std::endl;
                subscribers[msg.getDestinationApp()]->push(msg);
            }
        }
        while (lchMailBox_.size() > 0) {
            AirplugMessage msg = lchMailBox_.pop();
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
                airOutMailBox_->push(msg);
            }
        }
        while (airInMailBox_.size() > 0) {
            AirplugMessage msg = airInMailBox_.pop();
            std::cout << id_ << ": Received this message from AIR: " << msg.serialize() << std::endl;
            subscribers[msg.getDestinationApp()]->push(msg);
        }
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
    }
}
