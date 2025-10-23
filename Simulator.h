#pragma once

#include <vector> // TODO: mudar para list, a fim de melhorar eficiencia de tempo
#include <fstream>
#include <sstream>
#include <algorithm>
#include <climits>

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
    void removeTask(unsigned int idTask);

    void setAlgorithmScheduler(int i); // Usado por Menu.cpp
    void setAlgorithmScheduler(std::string algorithm);

    void setQuantum(unsigned int q);

    std::vector<TCB> getTasks() const;
    std::string getAlgorithmScheduler() const;
    unsigned int getQuantum() const;

    double calcTicksPerSecond();
    unsigned int sumDurationTasks();
    std::vector<unsigned int> getIdTasks();
    unsigned int getMaxEntryTime();

    const bool canAnyTaskEnter(double timeNow, unsigned int* indexTask, const unsigned int& exceptionIdTask);

    void trim(std::string &s);
    void remove_cr(std::string &s);
};