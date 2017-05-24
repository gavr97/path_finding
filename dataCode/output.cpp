#include <iostream>
#include "output.h"

Output::Output(double cellSize)
{
    time = 0;
    numberOfNodesCreated = 0;
    numberOfSteps = 0;
    numberOfMovements = 0;
    lengtnPath = 0;
    isLowLevel = true;
    this->cellSize = 1;
    std::cout << "warning: cellSize is assigned 0 despite custom specification\n";
}

void Output::printPath(const std::vector<Node> &path)
{
    if (path.size() == 0) {
        std::cout << "error: there is no inited path\n";
        return;
    } else {
        // do not forget about transposing and shift
        for (int ind = path.size() - 1; ind != -1; --ind) {
            std::cout << "number " << path.size() - ind - 1 << ": "
                      << path[ind].getY() - 1 << ' ' << path[ind].getX() - 1 << '\n';
        }
    }
}
