//
// Created by alexgavr on 20.02.17.
//

#include <iostream>
#include "../tinyxml/tinyxml2.h"
#include "../global/globalTypes.h"
#include "../global/globalVars.h"
#include "map.h"
#include <cmath>

Map::Map()
{

}

bool Map::isObstacle(const Node &node) const
{
    return grid[node.getX()][node.getY()] == '1';
}

bool Map::isObstacle(unsigned x, unsigned y) const
{
    return grid[x][y] == '1';
}

bool Map::isAllowedFromTo(unsigned ux, unsigned uy, unsigned vx, unsigned vy) const
{
    // as we here, (vx, vy) - is not obstacle
    int diff = abs((int)ux - (int)vx) + abs((int)uy - (int)vy);
    if (diff == 2 && allowDiag == 0)
        return 0;
    if (diff == 1 || allowSqueeze == 1)
        return 1;

    // here dif == 2; allowDiag == 1, squeeze == 0, cut is any;
    unsigned firstx = ux, firsty = vy;
    unsigned secondx = vx, secondy = uy;
    if (cutCorners == 0) {
        return (!(this->isObstacle(firstx, firsty)) && !(this->isObstacle(secondx, secondy)));
    } else {
        return (!(this->isObstacle(firstx, firsty)) || !(this->isObstacle(secondx, secondy)));
    }
}

Map::Map(const char *nameIn)
{
    readMap(nameIn);
}

int Map::readInt(XMLNode *pRoot, const char *tag, unsigned int &destination, unsigned int DEFAULT = 0, bool obligatory = 0)  // code of error is returned
{
    XMLError eResult;
    XMLElement * pElement = pRoot->FirstChildElement(tag);
    if (pElement == nullptr) {
        if (obligatory) {
            std::cout << "error: obligatory tag " << tag << " is not found\n";
            return 1;  // exit(1);
        } else {
            destination = DEFAULT;
            std::cout << tag << " is assigned by default " << DEFAULT << "\n";
            return 0;
        }
    } else {
        eResult = pElement->QueryUnsignedText(&destination);
        if (eResult != XML_SUCCESS) {
            std::cout << "error: format of " << tag << " is incorrect\n";
            return 1;  // exit(1);
        }
        //std::cout << tag << " is saved correctly\n";
    }
    return 0;
}

int Map::readMap(const char *nameIn)
{
    // ____load xml tree____
    XMLDocument xmlDoc;
    XMLError eResult;
    int myeResult;
    eResult = xmlDoc.LoadFile(nameIn);
    if (eResult != XML_SUCCESS) {
        std::cout << "error: incorrect xml file\n";
        return 1;
        //exit(1);
    }
    XMLNode *pRoot = xmlDoc.FirstChildElement("root");
    if (pRoot == nullptr){
        std::cout << "error: empty xml file\n";
        return 1;
        //exit(1);
    }

    // _______check obligatory tags________
    XMLNode *pMap = pRoot->FirstChildElement(TAG_MAP);
    if (pMap == nullptr) {
        std::cout << "error: incorrect structure of xml file: trere is no tag map\n";
        return 1;
        // exit(1);
    }
    XMLNode *pGrid = pMap->FirstChildElement(TAG_GRID);
    if (pGrid == nullptr) {
        std::cout << "error: incorrect structure of xml file: trere is no tag grid in subtree map\n";
        return 1;
    }
    XMLNode *pAlgorithm = pRoot->FirstChildElement(TAG_ALGORITHM);
    if (pAlgorithm == nullptr) {
        std::cout << "error: incorrect structure of xml file: trere is no tag algorithm\n";
        return 1;
        // exit(1);
    }

    if(readInt(pMap, TAG_HEIGHT, this->cntRealRows, 0, 1)) return 1;
    if(readInt(pMap, TAG_WIDTH, this->cntRealCols, 0, 1)) return 1;
    if(readInt(pAlgorithm, TAG_ALLOWDIAGONAL, this->allowDiag, ALLOW_DIAG_DEFAULT)) return 1;
    if(readInt(pAlgorithm, TAG_ALLOWSQUEEZE, this->allowSqueeze, ALLOW_SQUEEZE_DEFAULT)) return 1;
    if(readInt(pAlgorithm, TAG_CUTCORNERS, this->cutCorners, CUT_CORNERS_DEFAULT)) return 1;
    if ((allowDiag != 0 && allowDiag != 1) || (allowSqueeze != 0 && allowSqueeze != 1) ||
        (cutCorners != 0 && cutCorners != 1)) {
        std::cout << "error: allowdiag, allowsqueeze, cutcorners are incorrect\n";
        return 1;
    }
    if (allowDiag == 0 && (allowSqueeze == 1 || cutCorners == 1)) {
        std::cout << "error: allowdiag, allowsqueeze, cutcorners are not consistent\n";
        return 1;
    }
    if (allowSqueeze == 1 && cutCorners == 0) {
        std::cout << "error: allowsqueeze, cutcorners are not consistent\n";
        return 1;
    }
    std::cout << "specific information about task has been read succesfully\n";

    // _______read map___________
    Grid grid(this->cntRealRows + 2, GridRow(this->cntRealCols + 2));
    this->grid = grid;
    if(readGrid(pGrid, this->grid)) return 1;
    std::cout << "map has been read successfully\n" << std::endl;
    return 0;
}

int Map::readGrid(XMLNode *pGrid, Grid &grid)
{
    int height = grid.size() - 2;  // h = size - 2 because there is extra rows and cols for boundary
    int width = grid[0].size() - 2;
    int indRow = 0;
    int indCol = 0;

    //_____init boundaries______
    for (indCol = 0; indCol <= width + 1; ++indCol)
        grid[0][indCol] = OBSTACLE;
    indRow = height + 1;
    for (indCol = 0; indCol <= width + 1; ++indCol)
        grid[height + 1][indCol] = OBSTACLE;
    std::cout << "boundaries are inited\n";

    //______read map_______
    XMLElement *pRow = pGrid->FirstChildElement();
    indRow = 1;
    while (pRow != nullptr) {
        if (indRow == height + 1) {
            std::cout << "warning: too many rows\n";
            break;
        }
        const char *buf = pRow->GetText();
        grid[indRow][0] = OBSTACLE;
        grid[indRow][width + 1] = OBSTACLE;
        indCol = 1;
        while (*buf) {
            if (*buf == NO_OBSTACLE) {
                if (indCol == width + 1) {
                    std::cout << "warning: too many cells in row\n";
                    break;
                }
                grid[indRow][indCol] = NO_OBSTACLE;
                ++indCol;
            } else if (*buf == OBSTACLE) {
                if (indCol == width + 1) {
                    std::cout << "warning: too many cells in row\n";
                    break;
                }
                grid[indRow][indCol] = OBSTACLE;
                ++indCol;
            } else  if (*buf != ' ') {
                std::cout << "warning: unexpected character in the grid on the "
                          << indRow << " row, which is not taken into account\n";
            }
            ++buf;
        }
        if (indCol < width + 1) {
            std::cout << "error: too few cells in row " << indRow << "\n";
            return 1;
        }
        pRow = pRow->NextSiblingElement("row");
        ++indRow;
    }
    if (indRow < height + 1) {
        std::cout << "error: too few rows\n";
        return 1;
    }
    return 0;
}
