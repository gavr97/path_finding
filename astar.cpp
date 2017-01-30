//
// Created by alexgavr on 07.01.17.
//

#include <vector>
#include <cmath>
#include <string>
#include "myTask.h"
#include "myLog.h"
#include "myHeap.h"
#include "astar.h"

inline int my_Max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

inline TypeValue straight(unsigned ux, unsigned uy, unsigned vx, unsigned vy)
{
    TypeValue ret = my_Max(abs((int)ux - (int)vx), abs((int)uy - (int)vy));
    return ret;
}

inline TypeValue euclid(unsigned ux, unsigned uy, unsigned vx, unsigned vy)
{
    return sqrt(pow(((int)ux - (int)vx), 2) + pow(((int)uy - (int)vy), 2));
}

inline TypeValue manhattan(unsigned ux, unsigned uy, unsigned vx, unsigned vy)
{
    TypeValue ret = abs((int)ux - (int)vx) + abs((int)uy - (int)vy);
    return ret;
}

inline TypeValue zero(unsigned ux, unsigned uy, unsigned vx, unsigned vy)
{
    return 0;
}

inline unsigned AStar::key(unsigned ux, unsigned uy)
{
    return ux * (cntRealCols + 2) + uy;
}

inline unsigned AStar::coordinateFirst(unsigned key)
{
    return key / (cntRealCols + 2);
}

inline unsigned AStar::coordinateSecond(unsigned key)
{
    return key % (cntRealCols + 2);
}

int AStar::solve(const Task &task, Log &log)
{
    //_____define heuristic_____
    if (task.metricType == "manhattan")
        heuristic = &manhattan;
    else if (task.metricType == "euclid")
        heuristic = &euclid;
    else {
        heuristic = &straight;
        if (task.allowDiag == 1) {
            std::cout << "error: straight is inadmissible heiristic when allowdiag = 1\n";
            exit(0);
        }
    }

    //_____define sizes of map (astar needs it)
    cntRealCols = task.cntRealCols;
    cntRealRows = task.cntRealRows;
    startX = task.startX;
    startY = task.startY;
    finishX = task.finishX;
    finishY = task.finishY;

    //_____define dx and dy weights______
    if (task.allowDiag == 1) {
        dxVec = {0, 0, 1, -1, 1, -1, -1, 1};
        dyVec = {1, -1, 0, 0, 1, -1, 1, -1 };
        weightVec = {task.lineCost, task.lineCost, task.lineCost, task.lineCost,
                        task.diagCost, task.diagCost, task.diagCost, task.diagCost};
    } else {
        dxVec = {0, 0, 1, -1};
        dyVec = {1, -1, 0, 0};
        weightVec = {task.lineCost, task.lineCost, task.lineCost, task.lineCost};
    }

    //_____algorithm AStar______
    if (computeGValues(task)) {
        path = constructPath(log);
        return 0;
    } else {
        std::cout << "\nThere is no any path to goal\n";
        return 1;
    }
}

bool AStar::computeGValues(const Task &task)
{
    Heap opened;  // conceptually Heap is a set of pairs (f-value, key)
    std::map<unsigned, bool> visited;
    std::map<unsigned, bool> closed;
    unsigned startX = task.startX;
    unsigned startY = task.startY;
    unsigned finishX = task.finishX;
    unsigned finishY = task.finishY;
    unsigned keyNow = key(startX, startY);

    opened.push(0 + heuristic(startX, startY, finishX, finishY), keyNow);
    prevTable[keyNow] = {startX, startY, 0};
    gTable[keyNow] = 0;
    visited[keyNow] = true;
    while (!opened.empty()) {
        keyNow = opened.pop();
        closed[keyNow] = true;
        if (keyNow == key(finishX, finishY))
            return true;

        unsigned ux = coordinateFirst(keyNow), uy = coordinateSecond(keyNow);
        for (unsigned ind = 0; ind != dyVec.size(); ++ind) {
            unsigned vx = ux + dxVec[ind];
            unsigned vy = uy + dyVec[ind];
            unsigned keyNeig = key(vx, vy);
            if (task.map[vx][vy] == 0) {
                if (!visited[keyNeig]) {  // we must init gvalue of keyNeig
                    visited[keyNeig] = true;
                    TypeValue gVal = gTable[keyNow] + weightVec[ind];
                    opened.push(gVal + heuristic(vx, vy, finishX, finishY), keyNeig);
                    gTable[keyNeig] = gVal;
                    prevTable[keyNeig] = {coordinateFirst(keyNow), coordinateSecond(keyNow), weightVec[ind]};
                } else if (!closed[keyNeig]) {  // we must update gvalue
                    TypeValue gOld = gTable[keyNeig];
                    TypeValue gPretendent = gTable[keyNow] + weightVec[ind];
                    if (gOld > gPretendent) {
                        if (!opened.decreaseVal(gOld + heuristic(vx, vy, finishX, finishY),
                                                keyNeig, gPretendent + heuristic(vx, vy, finishX, finishY))) {
                            prevTable[keyNeig] = {coordinateFirst(keyNow), coordinateSecond(keyNow), weightVec[ind]};
                            gTable[keyNeig] = gPretendent;
                        } else {
                            std::cout << "error: failure during computation g-values\n";
                        }
                    }
                }
            }
        }
    }
    return false;
}

std::vector<unsigned> AStar::constructPath(Log &log)
{
    // Firstly, we must count steps in path
    unsigned nowKey = key(finishX, finishY);
    log.numberOfSteps = 0;
    log.length = 0;
    while (nowKey != key(startX, startY)) {
        Tripple nowTripple = prevTable[nowKey];
        nowKey = key(nowTripple.ux, nowTripple.uy);
        log.length += nowTripple.length;
        ++log.numberOfSteps;
    }
    unsigned number = log.numberOfSteps;
    log.initPath(finishX, finishY, number);
    --number;

    nowKey = key(finishX, finishY);
    while (nowKey != key(startX, startY)) {
        path.push_back(nowKey);
        Tripple nowTripple = prevTable[nowKey];
        log.addNode(nowTripple.ux, nowTripple.uy, nowTripple.length, number);
        nowKey = key(nowTripple.ux, nowTripple.uy);
        --number;
    }
    log.finish();
    return path;
}

void AStar::printPath()
{
    if (path.size() == 0) {
        std::cout << "error: there is no inited path\n";
        return;
    } else {
        for (int ind = path.size() - 1; ind != -1; --ind) {
            std::cout << "number " << path.size() - ind << ": "
                      << coordinateSecond(path[ind]) - 1 << ' ' << coordinateFirst(path[ind]) - 1 << '\n';  // agsin transposing and --
                                                                                                    // for user
        }
    }
}
