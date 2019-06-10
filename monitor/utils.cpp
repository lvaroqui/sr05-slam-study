#include "utils.h"
#include <sstream>
#include <iostream>
#include <math.h>

#define ROB_SIZE 25
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


int coordToMap (int coord)
{
    int modulo = coord % ROB_SIZE;
    int map= coord / ROB_SIZE;
    if(modulo < ROB_SIZE/2 && modulo > ROB_SIZE/2)
        map = modulo;
    else if (modulo <=ROB_SIZE/2)
        map-- ;
    else
        map++;

}

int floorNeg(int num) {
    if (num > 0) {
        return floor(num);
    }
    else {
        return -floor(-num);
    }
}
