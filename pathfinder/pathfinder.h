#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <list>
#include <utility>
#include <map>
#include <set>

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
    std::map<std::pair<float,float>, Node> _nodeMap;
    std::map<std::pair<float,float>,float> _cacheHeuristic;
    float calculateHeuristic(const std::pair<float, float> &pos, const std::pair<float, float> &end);
    float heavyHeuristic(const std::pair<float, float> &pos, const std::pair<float, float> &end);
    float heuristic(const std::pair<float,float> &pos, const std::pair<float,float> &end);
    std::pair<float,float> smallestFInOpenSet(const std::set<std::pair<float, float> > &openSet) const;
public:
    Pathfinder();
    void mapToNodeMap(const std::list<std::pair<float,float>> &map);
    std::list<std::pair<float, float>> findPath(const std::pair<float, float> &pos, const std::pair<float, float> &dest);
    std::list<std::pair<float, float>> goHome(const std::pair<float, float> &pos);
};

#endif // PATHFINDER_H
