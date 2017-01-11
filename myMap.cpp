//
// Created by alexgavr on 07.01.17.
//
#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include "tinyxml2.h"
#include "myMap.h"

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("TinyXML Error: %i\n", a_eResult); return a_eResult; }
#endif
#ifndef MYCheckResult
#define MYCheckResult(eResult) if (eResult != 0) {printf("MY Error: %i\n", eResult); return eResult; }
#endif

using namespace tinyxml2;


static double CELLSIZE_DEFAULT = 1;
static double LINE_COST_DEFAULT = 1;
static double DIAG_COST_DEFAULT = 1.414;
static unsigned int STARTX_DEFAULT = 0;
static unsigned int STARTY_DEFAULT = 0;
static unsigned int FINISHX_DEFAULT = 0;
static unsigned int FINISHY_DEFAULT = 0;
static const std::string SEARCH_TYPE_DEFAULT = "astar";
static const std::string METRIC_TYPE_DEFAULT = "euclid";
static unsigned int ALLOW_DIAG_DEFAULT = 0;
static unsigned int ALLOW_SQUEEZE_DEFAULT = 0;
static unsigned int CUT_CORNERS_DEFAULT = 0;

bool isValidSizes(Task task)
{
    bool positive1 = (task.cntRealCols > 0 && task.cntRealRows > 0);
    bool positive2 = (task.finishY > 0 && task.finishX > 0 && task.startX > 0 && task.startY > 0);
    return positive1 && positive2 &&
           task.startX <= task.cntRealRows && task.startY <= task.cntRealCols &&
           task.finishX <= task.cntRealRows && task.finishY <= task.cntRealCols;
}

int readInt(XMLNode *pRoot, const char *tag, unsigned int &destination, unsigned int DEFAULT = 0, bool obligatory = 0)  // code of error is returned
{
    XMLError eResult;
    XMLElement * pElement = pRoot->FirstChildElement(tag);
    if (pElement == nullptr) {
        if (obligatory) {
            std::cout << "error: obligatory tag " << tag << " is not found\n";
            exit(1);
        } else {
            destination = DEFAULT;
            std::cout << tag << " is assigned by default " << DEFAULT << "\n";
            return 0;
        }
    } else {
        eResult = pElement->QueryUnsignedText(&destination);
        if (eResult != XML_SUCCESS) {
            std::cout << "error: format of " << tag << " is incorrect\n";
            exit(1);
        }
        //std::cout << tag << " is saved correctly\n";
    }
    return 0;
}

int readDouble(XMLNode *pRoot, const char *tag, double &destination, double DEFAULT = 0, bool obligatory = 0)  // code of error is returned
{
    XMLError eResult;
    XMLElement * pElement = pRoot->FirstChildElement(tag);
    if (pElement == nullptr) {
        if (obligatory) {
            std::cout << "error: obligatory tag " << tag << " is not found\n";
            exit(1);
        } else {
            destination = DEFAULT;
            std::cout << tag << " is assigned by default " << DEFAULT << "\n";
            return 0;
        }
    } else {
        eResult = pElement->QueryDoubleText(&destination);
        if (eResult != XML_SUCCESS) {
            std::cout << "error: format of " << tag << " is incorrect\n";
            exit(1);
        }
        //std::cout << tag << " is saved correctly\n";
    }
    return 0;
}

int readStr(XMLNode *pRoot, const char *tag, std::string &destination, const std::string DEFAULT = 0,
            bool obligatory = 0)  // code of error is returned
{
    XMLError eResult;
    XMLElement * pElement = pRoot->FirstChildElement(tag);
    if (pElement == nullptr) {
        if (obligatory) {
            std::cout << "error: obligatory tag " << tag << " is not found\n";
            exit(1);
        } else {
            destination = DEFAULT;
            std::cout << tag << " is assigned by default " << DEFAULT << "\n";
            return 0;
        }
    } else {
        if (pElement->GetText() == nullptr) {
            std::cout << "error: empty " << tag << std::endl;
            exit(1);
        }
        destination = pElement->GetText();
    }
    return 0;
}

int readMap(XMLNode * pRoot, std::vector<std::vector<short>> &map)
{
    int height = map.size() - 2;  // h = size - 2 because there is extra rows and cols for boundary
    int width = map[0].size() - 2;
    int indRow = 0;
    int indCol = 0;

    //_____init boundaries______
    for (indRow : std::vector<int> {0, height + 1})
        for (indCol = 0; indCol <= width + 1; ++indCol)
            map[indRow][indCol] = 1;
    std::cout << "boundaries are inited\n";

    //_____init grid____________
    XMLElement *pGrid = pRoot->FirstChildElement("grid");
    if (pGrid == nullptr) {
        std:: cout << "error: incorrect structure of XML file: failure during finding tag grid\n";
        exit(1);
    }
    XMLElement *pRow = pGrid->FirstChildElement();

    indRow = 1;
    while (pRow != nullptr) {
        if (indRow == height + 1) {
            std::cout << "warning: too many rows\n";
            return 0;
        }
        const char *buf = pRow->GetText();
        map[indRow][0] = 1;
        map[indRow][width + 1] = 1;
        indCol = 1;
        while (*buf) {
            if (*buf == '0') {
                if (indCol == width + 1) {
                    std::cout << "warning: too many cells in row\n";
                    return 0;
                }
                map[indRow][indCol] = 0;
                ++indCol;
            } else if (*buf == '1') {
                if (indCol == width + 1) {
                    std::cout << "warning: too many cells in row\n";
                    return 0;
                }
                map[indRow][indCol] = 1;
                ++indCol;
            }
            ++buf;
        }
        if (indCol < width + 1) {
            std::cout << "error: too few cells in row " << indRow << "\n";
            exit(1);
            //return 1;
        }
        pRow = pRow->NextSiblingElement("row");
        ++indRow;
    }
    if (indRow < height + 1) {
        std::cout << "error: too few rows\n";
        exit(1);
        //return 1;
    }
    return 0;
}

int myLoad(const char *path, Task &task) {
    // ____load xml tree____
    XMLError eResult;
    int myeResult;
    XMLDocument xmlDoc;
    eResult = xmlDoc.LoadFile(path);
    if (eResult != XML_SUCCESS) {
        std::cout << "error: incorrect xml file\n";
        exit(1);
    }
    XMLNode *pRoot = xmlDoc.FirstChild();
    if (pRoot == nullptr){
        std::cout << "error: empty xml file\n";
        exit(1);
    }
    std::cout << "XML has been read\n";

    // _______init dimensions of map and other information________
    if (pRoot->FirstChildElement("map") == nullptr) {
        std::cout << "error: incorrect structure of xml file: trere is no tag map\n";
        exit(1);
    }
    if (pRoot->FirstChildElement("algorithm") == nullptr) {
        std::cout << "error: incorrect structure of xml file: trere is no tag algorithm\n";
        exit(1);
    }
    myeResult = readInt(pRoot->FirstChildElement("map"), "height", task.cntRealRows, 0, 1);  // 0 is incidential; 1 means obligatory
    myeResult = readInt(pRoot->FirstChildElement("map"), "width", task.cntRealCols, 0, 1);
    myeResult = readInt(pRoot->FirstChildElement("map"), "startx", task.startX, STARTX_DEFAULT);
    myeResult = readInt(pRoot->FirstChildElement("map"), "starty", task.startY, STARTY_DEFAULT);
    myeResult = readInt(pRoot->FirstChildElement("map"), "finishx", task.finishX, FINISHX_DEFAULT);
    myeResult = readInt(pRoot->FirstChildElement("map"), "finishy", task.finishY, FINISHY_DEFAULT);
    if (!isValidSizes(task)) {
        std::cout << "error: start or finish are inappropriate for these sizes\n";
        exit(1);
    }
    myeResult = readDouble(pRoot->FirstChildElement("algorithm"), "linecost", task.lineCost, LINE_COST_DEFAULT);
    myeResult = readDouble(pRoot->FirstChildElement("algorithm"), "diagonalcost", task.diagCost, DIAG_COST_DEFAULT);
    myeResult = readStr(pRoot->FirstChildElement("algorithm"), "searchtype", task.searchType, SEARCH_TYPE_DEFAULT);
    myeResult = readStr(pRoot->FirstChildElement("algorithm"), "metrictype", task.metricType, METRIC_TYPE_DEFAULT);

    myeResult = readInt(pRoot->FirstChildElement("algorithm"), "allowdiagonal", task.allowDiag, ALLOW_DIAG_DEFAULT);
    myeResult = readInt(pRoot->FirstChildElement("algorithm"), "allowsqueeze", task.allowSqueeze, ALLOW_SQUEEZE_DEFAULT);
    myeResult = readInt(pRoot->FirstChildElement("algorithm"), "cutcorners", task.cutCorners, CUT_CORNERS_DEFAULT);

    std::cout << "task has been read succesfully\n";

    // _______read map___________
    unsigned int height = task.cntRealRows;
    unsigned int width = task.cntRealCols;
    std::vector<std::vector<short>> map(height + 2, std::vector<short>(width + 2));
    myeResult = readMap(pRoot->FirstChildElement("map"), map);
    task.map = map;
    std::cout << "map has been read successfully\n";
    return 0;
}
