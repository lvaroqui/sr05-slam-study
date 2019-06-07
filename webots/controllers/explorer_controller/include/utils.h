#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>


bool fromStringToBool(const std::string& value);
std::string fromBoolToString(const bool& value);
std::vector<int> fromStringToVector(const std::string& value);
std::string fromVectorToString(std::vector<int> const& values);

std::string fromVectorOfPairsToString(std::vector<std::pair<int,int>> const& values );
std::vector<std::pair<int,int>> fromStringToVectorOfPairs(std::string const& value);

#endif // UTILS_H
