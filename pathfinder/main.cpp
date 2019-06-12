#include <iostream>
#include "pathfinder.h"
#include "../monitor/utils.h"
using namespace std;

int main()
{
    Pathfinder path;

//    std::cout<< path.heuristic(1,2) << std::endl;
//    std::cout<< path.heuristic2(std::pair<int,int>(1,2),std::pair<int,int>(0,0))<< std::endl;
//    std::cout<< path.heuristic2(std::pair<int,int>(2,3),std::pair<int,int>(1,1))<< std::endl;
//    std::cout<< path.heuristic2(std::pair<int,int>(2,3),std::pair<int,int>(-3,5))<< std::endl;

//    return 0;
    std::map<std::pair<int,int>, pointType> map;
    std::map<std::pair<int,int>, pointType> emptymap;
    emptymap.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(5,6), explored));

    //map.insert(map.begin(), std::pair<int,int>(1,1));
//    map.insert(map.begin(), std::pair<int,int>(4,1));
//    map.insert(map.begin(), std::pair<int,int>(4,-1));
//    map.insert(map.begin(), std::pair<int,int>(5,1));
//    map.insert(map.begin(), std::pair<int,int>(5,0));
//    map.insert(map.begin(), std::pair<int,int>(5,-1));
//    map.insert(map.begin(), std::pair<int,int>(3,1));
//    map.insert(map.begin(), std::pair<int,int>(3,0));
//    map.insert(map.begin(), std::pair<int,int>(3,-1));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(3,0), wall));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(3,-1), wall));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(3,1), wall));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(0,0), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(1,-1), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(1,-2), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(1,-3), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(2,-3), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(3,-3), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(4,-3), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(5,-3), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(6,-3), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(6,-2), explored));
    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(6,-1), explored));

//    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(3,0), wall));
//    map.insert(std::pair<std::pair<int,int>, pointType>(std::pair<int,int>(3,0), wall));
//    map.insert(map.begin(), std::pair<int,int>(2,1));
//    map.insert(map.begin(), std::pair<int,int>(2,2));
//    map.insert(map.begin(), std::pair<int,int>(2,-1));
//    map.insert(map.begin(), std::pair<int,int>(2,-2));
//    map.insert(map.begin(), std::pair<int,int>(2,3));
//    map.insert(map.begin(), std::pair<int,int>(50,-25));
//    map.insert(map.begin(), std::pair<int,int>(75,50));
    //map.insert(map.begin(), std::pair<int,int>(75,-50));
//    std::list<int> map;
//    map.insert(map.begin(), 1);
//    map.insert(map.begin(), 2);
    for(auto point = map.begin(); point != map.end(); ++point) {
        cout << point->first.first << point->first.first << endl;
    }
    cout << "Hello World!" << endl;
    std::set<std::pair<int,int>> opset;

    path.mapToNodeMap(emptymap);

    //    path.goHome(std::pair<int,int>(3,4));
//    path.goHome(std::pair<int,int>(4,4));
//    path.goHome(std::pair<int,int>(-1,4));
//    path.goHome(std::pair<int,int>(1,4));
//    path.goHome(std::pair<int,int>(1,-4));
//    path.goHome(std::pair<int,int>(-1,-4));
//    path.goHome(std::pair<int,int>(-4,1));
//    path.goHome(std::pair<int,int>(4,1));
//    path.goHome(std::pair<int,int>(-4,-1));
//    path.goHome(std::pair<int,int>(4,-1));
    //path.goHome(std::pair<int,int>(6,0));
    auto list = path.findPath(std::make_pair(3,3), std::make_pair(9,9));
    for (auto& a : list) {
        std::cout << a.first << a.second << "-";
    }
    //path.findPath(std::pair<int,int>(3,3), std::pair<int,int>(9,9));

    //    for(auto i = path._nodeMap.begin(); i!= path._nodeMap.end(); i++) {
//        std::cout << "point " << i->first.first << "," << i->first.second << ":" << i->second.hCost << std::endl;
//        if(i->second.hCost == -FLT_MAX) {
//            std::cout << "found it" << std::endl;
//        }
//    }
    Pathfinder path2;
    path2.mapToNodeMap(emptymap);
    path2.goHome(std::pair<int,int>(6,0), true);
    //path2.findPath(std::pair<int,int>(6,0), std::pair<int,int>(8,0));
   // path.goHome(std::pair<int,int>(1,0));
//    opset.insert(std::pair<int,int>(4,-1));
//    opset.insert(std::pair<int,int>(4,0));
//    opset.insert(std::pair<int,int>(1,0));
//    auto small = path.smallestFInOpenSet(opset);
//    cout << "Smallest : " << small.first <<" " << small.second << endl;
    std::cout << "Begining print cache" << std::endl;
//    for(auto i = path._cacheHeuristic.begin(); i!= path._cacheHeuristic.end(); i++) {
//        std::cout << "point " << i->first.first << "," << i->first.second << ":" << i->second << std::endl;
//    }
    Pathfinder pathfinder;
    pathfinder.mapToNodeMap(emptymap);
    auto paths = pathfinder.findPath(std::make_pair(5,6), std::make_pair(9,9));
    for (auto point : paths) {
        std::cout << point.first << point.second << "-";
    }
    return 0;
}
