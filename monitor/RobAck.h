//
// Created by luc on 30/05/19.
//

#ifndef EXPLORER_ROBACK_H
#define EXPLORER_ROBACK_H

#include <string>
#include <vector>
#include <airplug.h>

//#include <boost/algorithm/string/split.hpp>
//#include "AirplugMessage.h"

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
    std::vector<double> command_;
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

            int pos = static_cast<int>(param.find(','));

            while(pos != string::npos) {
                string substr = param.substr(0,pos);
                tmp.push_back(substr);

                param.erase(0, static_cast<int>(pos+1));
                pos = static_cast<int>(param.find(','));
            }
            tmp.push_back(param);

            for (auto elem : tmp)
                command_.push_back(stod(elem));
        }

    }


    Type getType() {
        return type_;
    }

    std::vector<double> getCommand() {
        return command_;
    }
};


#endif //EXPLORER_ROBACK_H
