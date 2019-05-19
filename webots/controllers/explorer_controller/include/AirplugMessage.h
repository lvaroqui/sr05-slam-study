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
    std::map<string, string> container_;
public:
    AirplugMessage() = default;

    explicit AirplugMessage(std::string message) {
        // Getting Mnemonics/Values from message
        std::vector<string> mnemo_valuePair;
        boost::split(mnemo_valuePair, message, [](char c) { return c == '^'; });
        mnemo_valuePair.erase(mnemo_valuePair.begin());

        // Adding the Mnemonics/Values to the map
        for (const auto &elem : mnemo_valuePair) {
            int pos = static_cast<int>(elem.find('~'));
            string mnemonic = elem.substr(0, pos);
            string value = elem.substr(pos + 1);
            container_[mnemonic] = value;
        }
    }

    string getValue(const string& mnemonic) {
        return container_[mnemonic];
    }

    void add(const string& mnemonic, string value) {
        container_[mnemonic] = value;
    }

    string serialize() {
        std::stringstream serialized;
        for(auto& elem : container_)
        {
            serialized << '^' << elem.first << '~' << elem.second;
        }
        return serialized.str();
    }
};


#endif //EXPLORER_AIRPLUGMESSAGE_H
