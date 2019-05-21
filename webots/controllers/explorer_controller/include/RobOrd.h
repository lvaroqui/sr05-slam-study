//
// Created by luc on 21/05/19.
//

#ifndef EXPLORER_ROBORD_H
#define EXPLORER_ROBORD_H

#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using std::string;
#include <vector>

class RobOrd {
public:
    enum Type {
        undefinied,
        move,
        turn,
        tune,
        lacc,
        init,
        curr,
        join
    };
private:
    Type type_ = undefinied;
    std::vector<int> command_;
public:
    RobOrd(string message) {
        int pos = static_cast<int>(message.find(':'));

        string order = message.substr(0, pos);
        string command = message.substr(pos + 1);

        if (order == "move") type_ = move;
        else if (order == "turn") type_ = turn;
        else if (order == "tune") type_ = tune;
        else if (order == "lacc") type_ = lacc;
        else if (order == "init") type_ = init;
        else if (order == "curr") type_ = curr;
        else if (order == "join") type_ = join;
        else type_ = undefinied;

        if (type_ == move || type_ == turn) {
            command_.push_back(stoi(command));
        }
        else if (type_ == tune || type_ == init || type_ == join) {
            std::vector<string> tmp;
            boost::split(tmp, message, [](char c) { return c == ','; });
            for (auto &elem : tmp) {
                command_.push_back(stoi(elem));
            }
        }
    }

    Type getType() {
        return type_;
    }

    std::vector<int> getCommand() {
        return command_;
    }
};


#endif //EXPLORER_ROBORD_H
