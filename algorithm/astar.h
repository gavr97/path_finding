//
// Created by alexgavr on 07.01.17.
//

/* path is vector<size_t> or
 *         vector<unsigned>
 *
 * every node has, naturally, indRow, indCol and at the same time key(its own integer identifier)
 *
 */

#ifndef REPO_ASTAR_H
#define REPO_ASTAR_H

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <bits/unordered_map.h>
#include "../dataCode/myTask.h"
#include "../algorithm/myHeap.h"
#include "../global/globalTypes.h"
#include "../dataCode/myOutput.h"
#include "../dataCode/Map.h"

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::pair;

class AStar {
private:
    //____members that define features of algorithm(object)_____
    TypeValue (*heuristicHide)(unsigned ux, unsigned uy, unsigned vx, unsigned vy);
    TypeValue heuristic(Node node1, Node node2);
    std::vector<short> dxVec;
    std::vector<short> dyVec;
    std::vector<TypeValue> weightVec;
    bool allowSqueeze();

    //____ commom members______
    unsigned cntRealCols;
    unsigned cntRealRows;
    unsigned startX;
    unsigned startY;
    unsigned finishX;
    unsigned finishY;

    std::map<Node, TypeValue> close;  // node->g-value
    Heap open;  // a set of pairs(f-value, Node)

    inline unsigned key(unsigned ux, unsigned uy);
    inline unsigned coordinateFirst(unsigned key);
    inline unsigned coordinateSecond(unsigned key);

    bool computeGValues(const Map &map, Output &output);
    int constructPath(Output &output);

public:
    int init (const Task &task);
    int solve(const Map &map, Output &output);
    void printClose();
};

#endif //REPO_ASTAR_H
