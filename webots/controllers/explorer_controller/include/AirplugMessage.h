#include <utility>

//
// Created by luc on 19/05/19.
//

#ifndef EXPLORER_AIRPLUGMESSAGE_H
#define EXPLORER_AIRPLUGMESSAGE_H


#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>

using std::string;

class AirplugMessage {
public:
    enum Type {
        undefined,
        local,
        air
    };

private:
    Type type_ = undefined;
    string destinationApp_ = "";
    std::map<string, string> container_;

public:
    AirplugMessage() = default;

    AirplugMessage(Type type, string destinationApp) : type_(type), destinationApp_(std::move(destinationApp)) {

    }

    explicit AirplugMessage(std::string message) {
        // Extracting App and Type
        int pos = static_cast<int>(message.find('^'));
        string tmp =  message.substr(0, pos);
        message = message.substr(pos+1);

        pos = static_cast<int>(tmp.find('_'));
        destinationApp_ = tmp.substr(0, pos);
        string type = tmp.substr(pos + 1);
        if (tmp == "LOC") type_ = local;
        else if (tmp == "AIR") type_ = air;
        else type_ = undefined;

        // Getting Mnemonics/Values from message
        std::vector<string> mnemo_valuePair;
        boost::split(mnemo_valuePair, message, [](char c) { return c == '^'; });

        // Adding the Mnemonics/Values to the map
        for (const auto &elem : mnemo_valuePair) {
            pos = static_cast<int>(elem.find('~'));
            string mnemonic = elem.substr(0, pos);
            string value = elem.substr(pos + 1);
            container_[mnemonic] = value;
        }
    }

    string getDestinationApp() {
        return destinationApp_;
    }

    Type getType() {
        return type_;
    }

    string getValue(const string &mnemonic) {
        return container_[mnemonic];
    }

    void add(const string &mnemonic, string value) {
        container_[mnemonic] = value;
    }

    void remove(const string &mnemonic) {
        container_.erase(mnemonic);
    }

    string serialize() {
        std::stringstream serialized;
        serialized << destinationApp_;
        switch (type_) {
            case undefined:
                serialized << "UND";
                break;
            case local:
                serialized << "LOC";
                break;
            case air:
                serialized << "AIR";
                break;
        }
        for (auto &elem : container_) {
            serialized << '^' << elem.first << '~' << elem.second;
        }
        return serialized.str();
    }
};


#endif //EXPLORER_AIRPLUGMESSAGE_H
