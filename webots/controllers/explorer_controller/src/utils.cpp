#include "utils.h"
#include <sstream>

using namespace  std;

bool fromStringToBool(const std::string& value)
{
    if (value == string("True")) return true;
    return false;
}

string fromBoolToString(const bool& value)
{
    if(value == true)
        return "True";
    return "False";
}

std::vector<int> fromStringToVector(const std::string& value)
{
    stringstream numbers(value);
    int number;
    vector<int> vectorCreated;
    while (numbers >> number)
        vectorCreated.push_back(number);
    return vectorCreated;
}
string fromVectorToString(std::vector<int> const& values)
{
    string vector;
    for(int i = 0;i<(int)values.size();i++)
    {
        vector += to_string(values[i]) + " ";
    }
    return vector;
}

std::string fromVectorOfPairsToString(std::vector<std::pair<int,int>> const& values )
{
    string vector;
    for(int i = 0;i<(int)values.size();i++)
    {
        vector += to_string(values[i].first)+ "," + to_string(values[i].second) + " ";
    }
    return vector;

}
std::vector<std::pair<int,int>> fromStringToVectorOfPairs(std::string const& value)
{
    stringstream stream(value);
    string numbers;
    vector<pair<int,int>> vectorCreated;
    pair<int,int> tmp;

    while (stream >> numbers)
    {
        int pos = numbers.find(',');
        string firstNumber = numbers.substr(0,pos);
        string secondNumber = numbers.substr(pos+1);
        tmp.first = stoi(firstNumber.c_str());
        tmp.second = stoi(secondNumber.c_str());
        vectorCreated.push_back(tmp);
    }
    return vectorCreated;
}
