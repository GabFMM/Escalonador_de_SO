#pragma once

#include <vector>
#include <fstream>
#include <sstream>

#include "Scheduler.h"
#include "GanttChartGenerator.h"
#include "TCB.h"
#include "ExtraInfo.h"
#include "FIFO.h"
#include "SRTF.h"
#include "PreemptivePriority.h"

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
    void addTask(TCB task);

    void setAlgorithmScheduler(int i); // Usado em Menu.cpp
    void setAlgorithmScheduler(std::string algorithm);

    std::vector<TCB> getTasks() const;
    std::string getAlgorithmScheduler() const;
    unsigned int getQuantum() const;
};