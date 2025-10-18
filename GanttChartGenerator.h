#pragma once

#include <ostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "TCB.h"

class GanttChartGenerator{
private:
    std::ofstream imageSVG;
    std::ostringstream rectsTasks;
    std::ostringstream axis;

public:
    GanttChartGenerator();
    ~GanttChartGenerator();

    void createAxis(size_t numTasks, std::vector<unsigned int> idTasks, unsigned int sumDurationTasks);

    void addRectTask(TCB& task);
};