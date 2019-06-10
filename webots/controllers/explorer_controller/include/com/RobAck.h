//
// Created by luc on 30/05/19.
//

#ifndef EXPLORER_ROBACK_H
#define EXPLORER_ROBACK_H

#include <string>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include "AirplugMessage.h"

using std::string;

class RobAck {
public:
    enum Type {
        undefinied,
        moved,
        turned,
        joined,
        lacc,
        init,
        curr,
        tuned,
        order
    };
private:
    Type type_ = undefinied;
    std::vector<int> command_;
public:
    explicit RobAck(string message) {
        int pos = static_cast<int>(message.find(':'));

        string ack = message.substr(0, pos);
        string param = message.substr(pos + 1);

        if (ack == "moved") type_ = moved;
        else if (ack == "turned") type_ = turned;
        else if (ack == "tuned") type_ = tuned;
        else if (ack == "lacc") type_ = lacc;
        else if (ack == "init") type_ = init;
        else if (ack == "curr") type_ = curr;
        else if (ack == "joined") type_ = joined;
        else if (ack == "order") type_ = order;
        else type_ = undefinied;

        if (type_ == moved || type_ == turned) {
            command_.push_back(stoi(param));
        }
        else if (type_ == tuned || type_ == init || type_ == joined || type_ == curr) {
            std::vector<string> tmp;
            boost::split(tmp, param, [](char c) { return c == ','; });
            for (auto &elem : tmp) {
                command_.push_back(stoi(elem));
            }
        }
    }

    static AirplugMessage turnedMsg(int value) {
        AirplugMessage msg("ROB", "EXP", AirplugMessage::local);
        msg.add("roback", "turned:" + std::to_string(value));
        return msg;
    }

    static AirplugMessage movedMsg(int value) {
        AirplugMessage msg("ROB", "EXP", AirplugMessage::local);
        msg.add("roback", "moved:" + std::to_string(value));
        return msg;
    }

    static AirplugMessage joinedMsg(int x, int y) {
        AirplugMessage msg("ROB", "EXP", AirplugMessage::local);
        msg.add("roback", "joined:" + std::to_string(x) + "," + std::to_string(y));
        return msg;
    }

    static AirplugMessage currMsg(int x, int y, int heading) {
        AirplugMessage msg("ROB", "EXP", AirplugMessage::local);
        msg.add("roback", "curr:" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(heading));
        return msg;
    }

    static void addRobotPosMsg(AirplugMessage &msg, int x, int y, int heading) {
        msg.add("robotpos", std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(heading));
    }

    static AirplugMessage initMsg(int x, int y, int heading) {
        AirplugMessage msg("ROB", "EXP", AirplugMessage::local);
        msg.add("roback", "init:" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(heading));
        return msg;
    }

    Type getType() {
        return type_;
    }

    std::vector<int> getCommand() {
        return command_;
    }
};


#endif //EXPLORER_ROBACK_H
