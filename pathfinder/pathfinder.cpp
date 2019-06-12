#include "pathfinder.h"

#include "../monitor/utils.h"
#include <limits>
#include <iostream>
#include <cmath>
#include <algorithm>

Pathfinder::Pathfinder()
{

}

float Pathfinder::calculateHeuristic(const std::pair<int, int> &pos, const std::pair<int, int> &end) {
    int newx = std::abs(pos.first-end.first);
    int newy = std::abs(pos.second-end.second);
    float t = (std::max(newx,newy)-std::abs(newx-newy))*1.4;
    float t2 = std::abs(newx-newy);
    //std::cout << "for point " << x << " "  << y << " : diagonale " << t << " lignedroite " << t2 << " total : " << t+t2 << std::endl;
    return (std::max(newx,newy)-std::abs(newx-newy))*1.4+std::abs(newx-newy);
}

float Pathfinder::heavyHeuristic(const std::pair<int,int> &pos, const std::pair<int,int> &end) {
// On suppose l'arrivée à 0,0
    //std::cout << "for point " << x << " "  << y << " : diagonale " << t << " lignedroite " << t2 << " total : " << t+t2 << std::endl;
    //std::cout << "passing by heavy";
    auto itcache = _cacheHeuristic.find(pos);
    if(itcache != _cacheHeuristic.end()) {
        return itcache->second;
    }
    std::map<std::pair<int,int>, Node>::iterator it;
    for(int i = -ROBOT_LENGTH/2; i<ROBOT_LENGTH/2;i++) {
        for(int j = -ROBOT_LENGTH/2; j<ROBOT_LENGTH/2;j++) {
            auto newCoord = std::pair<int,int>(pos.first+i,pos.second+j);
            //std::cout << newCoord.first << newCoord.second << std::endl;
            it = _nodeMap.find(newCoord);
            if(it != _nodeMap.end() && it->second.hCost == FLT_MAX) {
                std::cout << "Cache : "<< pos.first << pos.second << std::endl;
                _cacheHeuristic.insert(std::pair<std::pair<int,int>,float>(pos, FLT_MAX));
                return FLT_MAX;
            }
        }
    }
    return calculateHeuristic(pos, end);
// 1.4 ~= à sqrt(2), good enough pour heuristique
// à 1.4, on prend la diagonale, et à 1 on prend le côté
}

float Pathfinder::heuristic(const std::pair<int,int> &pos, const std::pair<int,int> &end) {
//    if(ROBOT_LENGTH != 1 || ROBOT_WIDTH != 1) {
//        return heavyHeuristic(pos,end);
//    }
//    int newx = std::abs(pos.first-end.first);
//    int newy = std::abs(pos.second-end.second);
//    float t = (std::max(newx,newy)-std::abs(newx-newy))*1.4;
//    float t2 = std::abs(newx-newy);
    //std::cout << "for point " << x << " "  << y << " : diagonale " << t << " lignedroite " << t2 << " total : " << t+t2 << std::endl;
    return calculateHeuristic(pos,end);
// 1.4 ~= à sqrt(2), good enough pour heuristique
// à 1.4, on prend la diagonale, et à 1 on prend le côté
}


void Pathfinder::mapToNodeMap(const std::map<std::pair<int, int>, pointType> &map) {
//    std::set<std::pair<int,int>> test;
    for(auto point = map.begin(); point != map.end(); ++point) {
        std::pair<int, int> newPoint = std::pair<int, int>(point->first.first/* / ROBOT_WIDTH*/, point->first.second /*/ ROBOT_LENGT)*/);
        if(point->second == wall) {
            _nodeMap.insert(std::pair<std::pair<int, int>, Node>(newPoint, Node(0,0,0,FLT_MAX)));
        } else {
            _nodeMap.insert(std::pair<std::pair<int, int>, Node>(newPoint, Node(0,0,0,-FLT_MAX)));
        }
//        test.insert(newPoint);
    }
    for(auto point = _nodeMap.begin(); point != _nodeMap.end(); ++point) {
        std::cout << "x : " << point->first.first << "y : " << point->first.second <<std::endl;
    }
//    auto node = test.find(std::pair<int,int>(0,1));
//    if(node != test.end()) {
//        std::cout << "In it mate" << node->first << node->second << std::endl;
//    }
//    node = test.find(std::pair<int,int>(0,2));
//    if(node != test.end()) {
//        std::cout << "In it mate" << node->first << node->second << std::endl;
//    } else {
//        std::cout << "Not in it mate"  << std::endl;
//    }
}

std::pair<int,int> Pathfinder::smallestFInOpenSet(const std::set<std::pair<int, int>> &openSet) const {
    if(openSet.empty()) {
        return std::pair<int,int>(INT_MAX, INT_MAX);
    }
    //std::pair<int,int> min;
    auto point = openSet.begin();
    auto min = _nodeMap.find(*point);
    if(min == _nodeMap.end()) {
        return std::pair<int,int>(INT_MAX, INT_MAX);
        //_nodeMap.insert(std::pair<std::pair<int,int>,Node>(*point, Node(0,0,0,heuristic(point->first,point->second))));
    }
    ++point;
    for(;point!= openSet.end();++point) {
        auto node = _nodeMap.find(*point);
        if(node == _nodeMap.end()) {
            return std::pair<int,int>();
            //_nodeMap.insert(std::pair<std::pair<int,int>,Node>(*point, Node(0,0,0,heuristic(point->first,point->second))));
        }
        if(node->second < min->second)
        {
            min = node;
        }
    }
    return min->first;
}

std::list<std::pair<int, int>> Pathfinder::goHome(const std::pair<int,int> &pos, bool onlyUsed) {
    return findPath(pos, std::pair<int,int>(0,0), onlyUsed);
}

std::list<std::pair<int, int> > Pathfinder::findPath(const std::pair<int, int> &pos, const std::pair<int, int> &dest, bool onlyUsed) {
    std::set<std::pair<int,int>> openSet, closedSet;
//  std::map<std::pair<int,int>,std::pair<int,int>> cameFrom;
    openSet.insert(pos);
    _nodeMap.insert(std::pair<std::pair<int, int>, Node>(pos, Node(pos.first,pos.second,0,heuristic(pos, dest))));

//    for(auto point = _nodeMap.begin(); point != _nodeMap.end(); ++point) {
// //        std::cout << "x : " << point->first.first << "y : " << point->first.second <<std::endl;
// //        std::cout << "Heuristique : " << point->second.fCost() <<std::endl;
//    }
    while(!openSet.empty()) {
        auto shortest = smallestFInOpenSet(openSet);
        auto current = _nodeMap.find(shortest);
        std::cout << "Instigated point " << current->first.first << " " << current->first.second << std::endl;
        if(current->first == dest) {
            printf("I found a way :");
            std::list<std::pair<int,int>> daWae;
            //auto currentPoint = std::pair<int,int>(0,0);
            auto currentNode = _nodeMap.find(current->first);
            while(currentNode->first != std::pair<int,int>(currentNode->second.parentX,currentNode->second.parentY)){
                daWae.insert(daWae.begin(),currentNode->first);
                currentNode = _nodeMap.find(std::pair<int,int>(currentNode->second.parentX,currentNode->second.parentY));
                std::cout << "Going back on " << currentNode->first.first << ":" << currentNode->first.second << std::endl;
            }
            daWae.insert(daWae.begin(),currentNode->first);
            std::cout << "Da Wae : ";
            for(auto point = daWae.begin(); point != daWae.end(); ++point) {
                std::cout << ":" << point->first << "," << point->second << ":" /*<< std::endl*/;
            }
            std::cout << std::endl;
            return daWae;
        }
        openSet.erase(shortest);
        closedSet.insert(shortest);
        for(int i = -1; i<=1;i++) {
            for(int j = -1; j <= 1; j++) {
                if(i==0 && j==0) {
                    continue;
                }
                auto newCoord = std::pair<int,int>(shortest.first+i,shortest.second+j);
                auto isClosedIterator = closedSet.find(newCoord);
                if(isClosedIterator != closedSet.end()) {
                    continue;
                }
                auto it = _nodeMap.find(newCoord);
                float newGCost = current->second.gCost + (i==0 || j ==0 ? 1.0 : 1.4);
                if(it != _nodeMap.end()) {
                    if(it->second.gCost <= newGCost && it->second.hCost != -FLT_MAX) {
                        continue;
                    } else if(it->second.hCost == -FLT_MAX) {
                        it->second.hCost = heuristic(newCoord, dest);
                        openSet.insert(newCoord);
                    }
                } else {
                    if(onlyUsed) {
                        continue;
                    }
                    float heur = heuristic(newCoord, dest);
                    //std::cout << heur << std::endl;
                    if(heur == FLT_MAX) {
                        continue;
                    }
                    openSet.insert(newCoord);
                    it = _nodeMap.insert(std::pair<std::pair<int,int>,Node>(newCoord, Node(0,0,0,heur))).first;
                }
                it->second.parentX = current->first.first;
                it->second.parentY = current->first.second;
                it->second.gCost = newGCost;
            }
        }
    }
    std::cout << "Didn't find a path" << std::endl;
    return std::list<std::pair<int,int>>();
}

void Pathfinder::resetMap() {
    _nodeMap.clear();
}
