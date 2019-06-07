#include <iostream>
#include "pathfinder.h"
using namespace std;

int main()
{
    Pathfinder path;

//    std::cout<< path.heuristic(1,2) << std::endl;
//    std::cout<< path.heuristic2(std::pair<float,float>(1,2),std::pair<float,float>(0,0))<< std::endl;
//    std::cout<< path.heuristic2(std::pair<float,float>(2,3),std::pair<float,float>(1,1))<< std::endl;
//    std::cout<< path.heuristic2(std::pair<float,float>(2,3),std::pair<float,float>(-3,5))<< std::endl;

//    return 0;
    std::list<std::pair<float,float>> map;
    //map.insert(map.begin(), std::pair<float,float>(1,1));
//    map.insert(map.begin(), std::pair<float,float>(4,1));
//    map.insert(map.begin(), std::pair<float,float>(4,-1));
//    map.insert(map.begin(), std::pair<float,float>(5,1));
//    map.insert(map.begin(), std::pair<float,float>(5,0));
//    map.insert(map.begin(), std::pair<float,float>(5,-1));
//    map.insert(map.begin(), std::pair<float,float>(3,1));
//    map.insert(map.begin(), std::pair<float,float>(3,0));
//    map.insert(map.begin(), std::pair<float,float>(3,-1));
    map.insert(map.begin(), std::pair<float,float>(2,0));
    map.insert(map.begin(), std::pair<float,float>(2,1));
    map.insert(map.begin(), std::pair<float,float>(2,2));
    map.insert(map.begin(), std::pair<float,float>(2,-1));
    map.insert(map.begin(), std::pair<float,float>(2,-2));
    map.insert(map.begin(), std::pair<float,float>(2,3));
    map.insert(map.begin(), std::pair<float,float>(50,-25));
    map.insert(map.begin(), std::pair<float,float>(75,50));
    map.insert(map.begin(), std::pair<float,float>(75,-50));
//    std::list<float> map;
//    map.insert(map.begin(), 1);
//    map.insert(map.begin(), 2);
    for(auto point = map.begin(); point != map.end(); ++point) {
        cout << point->first << point->second << endl;
    }
    cout << "Hello World!" << endl;
    std::set<std::pair<float,float>> opset;
    path.mapToNodeMap(map);
//    path.goHome(std::pair<float,float>(3,4));
//    path.goHome(std::pair<float,float>(4,4));
//    path.goHome(std::pair<float,float>(-1,4));
//    path.goHome(std::pair<float,float>(1,4));
//    path.goHome(std::pair<float,float>(1,-4));
//    path.goHome(std::pair<float,float>(-1,-4));
//    path.goHome(std::pair<float,float>(-4,1));
//    path.goHome(std::pair<float,float>(4,1));
//    path.goHome(std::pair<float,float>(-4,-1));
//    path.goHome(std::pair<float,float>(4,-1));
    path.goHome(std::pair<float,float>(4,0));

    Pathfinder path2;
    path2.mapToNodeMap(map);
    path2.findPath(std::pair<float,float>(4,0), std::pair<float,float>(6,0));
   // path.goHome(std::pair<float,float>(1,0));
//    opset.insert(std::pair<float,float>(4,-1));
//    opset.insert(std::pair<float,float>(4,0));
//    opset.insert(std::pair<float,float>(1,0));
//    auto small = path.smallestFInOpenSet(opset);
//    cout << "Smallest : " << small.first <<" " << small.second << endl;
    return 0;
}
