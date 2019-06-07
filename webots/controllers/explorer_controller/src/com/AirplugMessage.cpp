//
// Created by luc on 19/05/19.
//

#include "com/AirplugMessage.h"

string AirplugMessage::serialize() {
    std::stringstream serialized;
    serialized << "$";
    switch (type_) {
        case undefined:
            serialized << "UND";
            break;
        case local:
            serialized << "LCH";
            break;
        case air:
            serialized << "AIR";
            break;
    }
    serialized << "$" << emissionApp_ << "$" << destinationApp_ << "$";
    for (auto &elem : container_) {
        elem.second.erase(std::remove(elem.second.begin(), elem.second.end(), '\n'), elem.second.end());
        serialized << '^' << elem.first << '~' << elem.second;
    }
    return serialized.str();
}

AirplugMessage::AirplugMessage(std::string message) {
    std::vector<string> buffer;

    // Getting Apps and Type
    int pos = static_cast<int>(message.find('^'));
    string tmp =  message.substr(0, pos);
    message = message.substr(pos+1);

    boost::split(buffer, tmp, [](char c) { return c == '$'; });

    if (buffer[1] == "LCH") type_ = local;
    else if (buffer[1] == "AIR") type_ = air;
    else type_ = undefined;

    emissionApp_ = buffer[2];
    destinationApp_ = buffer[3];

    // Getting Mnemonics/Values from message
    buffer.clear();
    boost::split(buffer, message, [](char c) { return c == '^'; });

    // Adding the Mnemonics/Values to the map
    for (const auto &elem : buffer) {
        pos = static_cast<int>(elem.find('~'));
        string mnemonic = elem.substr(0, pos);
        string value = elem.substr(pos + 1);
        container_[mnemonic] = value;
    }
}
