#pragma once

#include <vector> // TODO: mudar para list, a fim de melhorar eficiencia de tempo
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <climits>

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
    ExtraInfo extraInfo;

    std::vector<TCB> tasks;

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

    double calcTicksPerSecond();
    unsigned int sumDurationTasks();
    std::vector<unsigned int> getIdTasks();
    unsigned int getMaxEntryTime();
};