#include <utility>

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
    string emissionApp_ = "";
    string destinationApp_ = "";
    Type type_ = undefined;
    std::map<string, string> container_;

public:
    AirplugMessage() = default;

    AirplugMessage(string emissionApp, string destinationApp, Type type)
            : emissionApp_(std::move(emissionApp)), destinationApp_(std::move(destinationApp)), type_(type) {

    }

    explicit AirplugMessage(std::string message) {
        std::vector<string> buffer;

        // Getting Apps and Type
        int pos = static_cast<int>(message.find('^'));
        string tmp =  message.substr(0, pos);
        message = message.substr(pos+1);

        boost::split(buffer, tmp, [](char c) { return c == '$'; });
        emissionApp_ = buffer[1];
        destinationApp_ = buffer[2];

        if (buffer[3] == "LOC") type_ = local;
        else if (buffer[3] == "AIR") type_ = air;
        else type_ = undefined;


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

    string getEmissionApp() {
        return destinationApp_;
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
        serialized << "$" << emissionApp_ << "$" << destinationApp_ << "$";
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
        serialized << "$";
        for (auto &elem : container_) {
            serialized << '^' << elem.first << '~' << elem.second;
        }
        return serialized.str();
    }
};


#endif //EXPLORER_AIRPLUGMESSAGE_H
