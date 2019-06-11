#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <map>
#include <utility>

#define ROB_SIZE 50
enum pointType {
    wall,
    explored,
    unexplored
};

typedef std::map<std::pair<int, int>, pointType > Map;

bool fromStringToBool(const std::string& value);
std::string fromBoolToString(const bool& value);
std::vector<int> fromStringToVector(const std::string& value);
std::string fromVectorToString(std::vector<int> const& values);

std::string fromVectorOfPairsToString(std::vector<std::pair<int,int>> const& values );
std::vector<std::pair<int,int>> fromStringToVectorOfPairs(std::string const& value);

std::string fromMapToString(Map const& map);
Map fromStringToMap(std::string const& str);

int floorNeg(int num);


int coordToMap (int coord);

#endif // UTILS_H
