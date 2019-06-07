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

    explicit AirplugMessage(std::string message);

    string getEmissionApp() {
        return emissionApp_;
    }

    string getDestinationApp() {
        return destinationApp_;
    }

    void setDestinationApp(string destinationApp) {
        destinationApp_ = destinationApp;
    }

    Type getType() {
        return type_;
    }

    const string getValue(const string &mnemonic) {
        return container_[mnemonic];
    }

    void add(const string &mnemonic, string value) {
        container_[mnemonic] = value;
    }

    void remove(const string &mnemonic) {
        container_.erase(mnemonic);
    }

    string serialize();
};


#endif //EXPLORER_AIRPLUGMESSAGE_H
