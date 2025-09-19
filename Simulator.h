#pragma once

#include <vector>

#include "Scheduler.h"
#include "GanttChartGenerator.h"
#include "TCB.h"
#include "ExtraInfo.h"

class Menu;

class Simulator
{
private:
    Menu* menu;
    Scheduler* scheduler;
    GanttChartGenerator* imageGenerator;
    ExtraInfo* extraInfo;

public:
    Simulator();
    ~Simulator();

    void start();
    void executeDebugger();
    void executeNoDebugger();
    std::vector<TCB> loadArquive();
    void generateImage();
};