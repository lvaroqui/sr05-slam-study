#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <list>
#include <utility>
#include <map>
#include <set>
#include "../monitor/utils.h"

#ifndef ROBOT_LENGTH
#define ROBOT_LENGTH 1
#define ROBOT_WIDTH 1
#endif

struct Node
{
private :
    float gWeight = 1;
    float hWeight = 1;
public :
    Node(int parentXX, int parentYY, float ggCost, float hhCost) : parentX(parentXX), parentY(parentYY), gCost(ggCost), hCost(hhCost) {}
    int parentX;
    int parentY;
    float gCost;
    float hCost;
    float fCost() const {return gWeight*gCost+hWeight*hCost;}
};

inline bool operator < (const Node& lhs, const Node& rhs)
{
    return lhs.fCost() < rhs.fCost();
}

class Pathfinder
{
private:
    std::map<std::pair<int,int>, Node> _nodeMap;
    std::map<std::pair<int,int>,float> _cacheHeuristic;
    float calculateHeuristic(const std::pair<int, int> &pos, const std::pair<int, int> &end);
    float heavyHeuristic(const std::pair<int, int> &pos, const std::pair<int, int> &end);
    float heuristic(const std::pair<int,int> &pos, const std::pair<int,int> &end);
    std::pair<int,int> smallestFInOpenSet(const std::set<std::pair<int, int> > &openSet) const;
public:
    std::list<std::pair<int, int>> smoothPath(const std::list<std::pair<int, int>>& path);
    Pathfinder();
    void resetMap();
    void mapToNodeMap(const std::map<std::pair<int,int>, pointType> &map);
    std::list<std::pair<int, int>> findPath(const std::pair<int, int> &pos, const std::pair<int, int> &dest, bool onlyUsed = false);
    std::list<std::pair<int, int> > goHome(const std::pair<int, int> &pos, bool onlyUsed = false);
};

#endif // PATHFINDER_H
