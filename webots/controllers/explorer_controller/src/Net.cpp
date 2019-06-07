//
// Created by luc on 21/05/19.
//

#include "Net.h"

void Net::run() {
    while (true) {
        while (udpServer_.getNumberOfMessages() > 0) {
            mailBox_.push(AirplugMessage(udpServer_.popMessage()));
        }
        while (mailBox_.size() > 0) {
            AirplugMessage msg = mailBox_.pop();
            std::cout << "Received: " << msg.serialize() << std::endl;
            if (msg.getDestinationApp() == "NET") {
                if (msg.getValue("typemsg") == "MAPCO") {
                    monitors_.emplace(msg.getValue("ip") + ":" + msg.getValue("port"), new UDPClient(msg.getValue("ip"), std::stoi(msg.getValue("port"))));
                    subscribers["EXP"]->push(msg);
                    std::cout << "New monitor at: " << msg.getValue("ip") << ":" << msg.getValue("port") << std::endl;
                }
                else if (msg.getValue("typemsg") == "ENDCO") {
                    monitors_.erase(msg.getValue("ip") + ":" + msg.getValue("port"));
                }
            }
            else if (msg.getDestinationApp() == "MAP") {
                for (auto monitor : monitors_) {
                    monitor.second->sendMessage(msg.serialize());
                }
            }
            else if (subscribers.count(msg.getDestinationApp()) == 1) {

                subscribers[msg.getDestinationApp()]->push(msg);
            }
        }
    }
}
