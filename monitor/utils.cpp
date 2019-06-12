#include "utils.h"
#include <sstream>
#include <iostream>
#include <math.h>


using namespace std;

bool fromStringToBool(const std::string &value) {
    if (value == string("True")) return true;
    return false;
}

string fromBoolToString(const bool &value) {
    if (value == true)
        return "True";
    return "False";
}

std::vector<int> fromStringToVector(const std::string &value) {
    stringstream numbers(value);
    int number;
    vector<int> vectorCreated;
    while (numbers >> number)
        vectorCreated.push_back(number);
    return vectorCreated;
}

string fromVectorToString(std::vector<int> const &values) {
    string vector;
    for (int i = 0; i < (int) values.size(); i++) {
        vector += to_string(values[i]) + " ";
    }
    return vector;
}

std::string fromVectorOfPairsToString(std::vector<std::pair<int, int>> const &values) {
    string vector;
    for (int i = 0; i < (int) values.size(); i++) {
        vector += to_string(values[i].first) + "," + to_string(values[i].second) + " ";
    }
    return vector;

}

std::vector<std::pair<int, int>> fromStringToVectorOfPairs(std::string const &value) {
    stringstream stream(value);
    string numbers;
    vector<pair<int, int>> vectorCreated;
    pair<int, int> tmp;

    while (stream >> numbers) {
        int pos = numbers.find(',');
        string firstNumber = numbers.substr(0, pos);
        string secondNumber = numbers.substr(pos + 1);
        tmp.first = stoi(firstNumber.c_str());
        tmp.second = stoi(secondNumber.c_str());
        vectorCreated.push_back(tmp);
    }
    return vectorCreated;
}

std::string fromMapToString(std::map<std::pair<int, int>, pointType> const &map) {
    if (map.empty()) {
        return "";
    }
    std::string str;
    for (auto point : map) {
        str += std::to_string(point.first.first) + "," + std::to_string(point.first.second) + "," + std::to_string(point.second) + " ";
    }
    str.pop_back();
    return str;
}

std::map<std::pair<int, int>, pointType> fromStringToMap(std::string const &str) {
    std::map<std::pair<int, int>, pointType> map;
    stringstream stream(str);
    string block;

    while (stream >> block) {
        int pos = block.find(',');
        string first = block.substr(0, pos);
        block = block.substr(pos + 1);
        pos = block.find(',');
        string second = block.substr(0, pos);
        string third = block.substr(pos + 1);
        map[std::make_pair(std::stoi(first), std::stoi(second))] = static_cast<pointType>(std::stoi(third));
    }
    return map;
}


std::string fromMapToStringClock(std::map<std::string, int> const &map) {
    if (map.empty()) {
        return "";
    }
    std::string str;
    for (auto clk : map) {
        str += clk.first + "," + std::to_string(clk.second) + " ";
    }
    str.pop_back();
    return str;
}

std::map<std::string, int> fromStringToMapClock(std::string const &str) {
    std::map<std::string, int> map;
    stringstream stream(str);
    string block;

    while (stream >> block) {
        int pos = block.find(',');
        string first = block.substr(0, pos);
        block = block.substr(pos + 1);
        pos = block.find(',');
        string second = block.substr(0, pos);
        map[first] = std::stoi(second);
    }
    return map;
}



int coordToMap (int coord)
{
    int modulo = abs(coord) % ROB_SIZE;
    int newCoord = abs(coord) / ROB_SIZE;
    if(modulo > ROB_SIZE/2)
        newCoord++;
    if (coord >= 0) {
        return newCoord;
    }
    else {
        return -newCoord;
    }
}

int floorNeg(int num) {
    if (num > 0) {
        return floor(num);
    }
    else {
        return -floor(-num);
    }
}

void findFrontiers(Map &map) {
    for(auto point : map) {
        if(point.second == explored) {
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    std::pair<int, int> coords = std::pair<int, int>(point.first.first + i, point.first.second + j);
                    if(map.find(coords) == map.end()) {
                        map[coords] = frontier;
                    }
                }
            }
        }
    }
}

bool findClosestFrontier(const Map &map, const std::pair<int, int> &position, std::pair<int, int> &bestFrontier) {
    bool found= false;
    double minDist = std::numeric_limits<double>::max();
    double wallNum = 0;
    for(auto point : map) {
        if(point.second == frontier) {
            found = true;
            double dist = std::sqrt(std::pow(point.first.first-position.first, 2) + std::pow(point.first.second-position.second, 2));
            if(dist <= minDist) {
                int walls = 0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        std::pair<int, int> coords = std::pair<int, int>(point.first.first + i, point.first.second + j);
                        auto it = map.find(coords);
                        if(it != map.end() && it->second == wall) {
                            walls++;
                        }
                    }
                }
                if (dist < minDist) {
                    wallNum = walls;
                    minDist = dist;
                    bestFrontier = point.first;
                }
                else if (walls > wallNum) {
                    wallNum = walls;
                    minDist = dist;
                    bestFrontier = point.first;
                }
            }
        }
    }
    return found;
}
