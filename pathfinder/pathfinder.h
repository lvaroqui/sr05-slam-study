#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <list>
#include <utility>
#include <map>
#include <set>

#define ROBOT_LENGTH 26
#define ROBOT_WIDTH 16


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
    std::map<std::pair<float,float>, Node> _nodeMap;
    float heuristic(int x, int y);
public:
    std::pair<float,float> smallestFInOpenSet(const std::set<std::pair<float, float> > &openSet) const;
public:
    Pathfinder();
    void mapToNodeMap(const std::list<std::pair<float,float>> &map);
    void goHome(const std::pair<float, float> &pos);
};

#endif // PATHFINDER_H
