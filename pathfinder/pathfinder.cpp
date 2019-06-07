#include "pathfinder.h"
#include <limits>
#include <iostream>
#include <cmath>
#include <algorithm>

Pathfinder::Pathfinder()
{

}

float Pathfinder::heuristic(int x, int y) {
// On suppose l'arrivée à 0,0
    int newx = std::abs(x);
    int newy = std::abs(y);
    float t = (std::max(newx,newy)-std::abs(newx-newy))*1.4;
    float t2 = std::abs(newx-newy);
    //std::cout << "for point " << x << " "  << y << " : diagonale " << t << " lignedroite " << t2 << " total : " << t+t2 << std::endl;
    return (std::max(newx,newy)-std::abs(newx-newy))*1.4+std::abs(newx-newy);
// 1.4 ~= à sqrt(2), good enough pour heuristique
// à 1.4, on prend la diagonale, et à 1 on prend le côté
}

float Pathfinder::heuristic2(const std::pair<float,float> &pos, const std::pair<float,float> &end) {
    int newx = std::abs(pos.first-end.first);
    int newy = std::abs(pos.second-end.second);
    float t = (std::max(newx,newy)-std::abs(newx-newy))*1.4;
    float t2 = std::abs(newx-newy);
    //std::cout << "for point " << x << " "  << y << " : diagonale " << t << " lignedroite " << t2 << " total : " << t+t2 << std::endl;
    return (std::max(newx,newy)-std::abs(newx-newy))*1.4+std::abs(newx-newy);
// 1.4 ~= à sqrt(2), good enough pour heuristique
// à 1.4, on prend la diagonale, et à 1 on prend le côté
}


void Pathfinder::mapToNodeMap(const std::list<std::pair<float, float> > &map) {
    std::set<std::pair<float,float>> test;
    for(auto point = map.begin(); point != map.end(); ++point) {
        std::pair<float, float> newPoint = std::pair<float, float>(floor(point->first/* / ROBOT_WIDTH*/), floor(point->second /*/ ROBOT_LENGT)*/));
        _nodeMap.insert(std::pair<std::pair<float, float>, Node>(newPoint, Node(0,0,0,FLT_MAX)));
        test.insert(newPoint);
    }
    for(auto point = _nodeMap.begin(); point != _nodeMap.end(); ++point) {
        std::cout << "x : " << point->first.first << "y : " << point->first.second <<std::endl;
    }
    auto node = test.find(std::pair<float,float>(0,1));
    if(node != test.end()) {
        std::cout << "In it mate" << node->first << node->second << std::endl;
    }
    node = test.find(std::pair<float,float>(0,2));
    if(node != test.end()) {
        std::cout << "In it mate" << node->first << node->second << std::endl;
    } else {
        std::cout << "Not in it mate"  << std::endl;
    }
}

std::pair<float,float> Pathfinder::smallestFInOpenSet(const std::set<std::pair<float, float>> &openSet) const {
    if(openSet.empty()) {
        return std::pair<float,float>(FLT_MAX, FLT_MAX);
    }
    //std::pair<float,float> min;
    auto point = openSet.begin();
    auto min = _nodeMap.find(*point);
    if(min == _nodeMap.end()) {
        return std::pair<float,float>(FLT_MAX, FLT_MAX);
        //_nodeMap.insert(std::pair<std::pair<float,float>,Node>(*point, Node(0,0,0,heuristic(point->first,point->second))));
    }
    ++point;
    for(;point!= openSet.end();++point) {
        auto node = _nodeMap.find(*point);
        if(node == _nodeMap.end()) {
            return std::pair<float,float>();
            //_nodeMap.insert(std::pair<std::pair<float,float>,Node>(*point, Node(0,0,0,heuristic(point->first,point->second))));
        }
        if(node->second < min->second)
        {
            min = node;
        }
    }
    return min->first;
}

void Pathfinder::goHome(const std::pair<float,float> &pos) {
    findPath(pos, std::pair<float,float>(0,0));
}

void Pathfinder::findPath(const std::pair<float, float> &pos, const std::pair<float, float> &dest) {
    std::set<std::pair<float,float>> openSet, closedSet;
//  std::map<std::pair<float,float>,std::pair<float,float>> cameFrom;
    openSet.insert(pos);
    _nodeMap.insert(std::pair<std::pair<float, float>, Node>(pos, Node(pos.first,pos.second,0,heuristic2(pos, dest))));

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
            std::list<std::pair<float,float>> daWae;
            //auto currentPoint = std::pair<float,float>(0,0);
            auto currentNode = _nodeMap.find(current->first);
            while(currentNode->first != std::pair<float,float>(currentNode->second.parentX,currentNode->second.parentY)){
                daWae.insert(daWae.begin(),currentNode->first);
                currentNode = _nodeMap.find(std::pair<float,float>(currentNode->second.parentX,currentNode->second.parentY));
                std::cout << "Going back on " << currentNode->first.first << ":" << currentNode->first.second << std::endl;
            }
            daWae.insert(daWae.begin(),currentNode->first);
            std::cout << "Da Wae : ";
            for(auto point = daWae.begin(); point != daWae.end(); ++point) {
                std::cout << ":" << point->first << "," << point->second << ":" /*<< std::endl*/;
            }
            std::cout << std::endl;
            return;
        }
        openSet.erase(shortest);
        closedSet.insert(shortest);
        for(int i = -1; i<=1;i++) {
            for(int j = -1; j <= 1; j++) {
                if(i==0 && j==0) {
                    continue;
                }
                auto newCoord = std::pair<float,float>(shortest.first+i,shortest.second+j);
                auto it = _nodeMap.find(newCoord);
                float newGCost = current->second.gCost + (i==0 || j ==0 ? 1.0 : 1.4);
                if(it != _nodeMap.end()) {
                    if(it->second.gCost <= newGCost) {
                        continue;
                    }
                } else {
                    openSet.insert(newCoord);
                    it = _nodeMap.insert(std::pair<std::pair<float,float>,Node>(newCoord, Node(0,0,0,heuristic2(newCoord, dest)))).first;
                }
                it->second.parentX = current->first.first;
                it->second.parentY = current->first.second;
                it->second.gCost = newGCost;
            }
        }
    }
}
