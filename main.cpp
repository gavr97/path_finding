#include <iostream>
#include <vector>
#include "dataCode/myTask.h"
#include "logger/myLog.h"
#include "dataCode/myOutput.h"
#include "algorithm/astar.h"

#include <fstream>
#include <ctime>
#include <cstring>

int main(int argc, char *argv[]) {
    //________specify input and output names________
    if (argc == 1) {
        std::cout << "error: input XML is not specified\n";
        return 0;
    }
    const char *name = argv[1];
    const char *nameOut;
    if (argc > 2) {
        nameOut = argv[2];
    } else {
        nameOut = "out.xml";
    }
    std:: cout << "name of input = " << name << std::endl;
    std:: cout << "name of output = " << nameOut << std::endl << std::endl;

    //________Init Task________
    int myeResult = 0;
    Task task; Output output; Log log;
    myeResult = task.myLoad(name, log);  // char* ! not std::string;
                                        // log is sent as an argument for storing XMLDoc for future out to a user
    if (myeResult) {
        std::cout << "task: " << name << " is skipped\n";
        return 0;
    }
    task.print();

    //_______Init AStar and Map_____
    Map map = task.map;
    AStar astar;
    myeResult = astar.init(task);
    if (myeResult) {
        std::cout << "error: failure during initializing astar\n";
        return 0;
    }

    //_______Solve Task_________
    myeResult = astar.solve(map, output);  // myeResult == 1 if no path found; else 0;
    if (myeResult) {
        std::cout << "no path found in task\n"
                  << "Naturally, Path is not saved in output\n";
    } else {
        std::cout << "path is found\n";
    }

    //_____Save results and write_____
    myeResult = log.saveData(output, name, map);  // arg-task.map is neccessary because of space-map
    log.write(nameOut);
    // на данный момент логгер уже ПРИВЯЗАН к текущей задаче(имеет поле xmlDoc, соответствующее этой задаче)
    // log.clean()??
    return 0;
}