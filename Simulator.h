#pragma once

#include <vector> // TODO: mudar para list, a fim de melhorar eficiencia de tempo
#include <fstream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <random>

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

    std::vector<TCB*> tasks;

public:
    Simulator();
    ~Simulator();

    void start();
    void executeDebugger();
    void executeNoDebugger();
    void executeDefault(TCB** currentTask, unsigned int* globalClock, const unsigned int* deltaTime, unsigned int* currentTaskQuantum, unsigned int* timeLastInterrupt);
    std::vector<TCB*> loadArquive();
    void generateImage();

    // manipulacao de tasks
    std::vector<TCB*> getTasks() const;
    std::vector<unsigned int> getIdTasks();
    void addTask(TCB task);
    void updateTask(const TCB* task);
    void updateTaskId(const unsigned int& idTask, const unsigned int& newId);
    void updateTaskColor(const unsigned int& idTask, const unsigned int& newColor);
    void updateTaskColor(const unsigned int& idTask, const std::string& newStrColor);
    void updateTaskEntryTime(const unsigned int& idTask, const unsigned int& newEntryTime);
    void updateTaskDuration(const unsigned int& idTask, const unsigned int& newDuration);
    void updateTaskStaticPriority(const unsigned int& idTask, const unsigned int& newPriority);

    // setters e getters
    void setAlgorithmScheduler(int i); // Usado por Menu.cpp
    void setAlgorithmScheduler(std::string algorithm);
    void setQuantum(unsigned int q);
    void setAlpha(unsigned int a);
    std::string getAlgorithmScheduler() const;
    unsigned int getQuantum() const;
    unsigned int getAlpha() const;

    double calcTicksPerSecond();
    unsigned int sumDurationTasks();
    unsigned int getMaxEntryTime();

    const bool canAnyTasksEnter(double timeNow, std::vector<unsigned int>& indexTasks);
    const bool IsThereAnUnfinishedTask();

    // usado no loadArquive
    void trim(std::string &s);
    void remove_cr(std::string &s);

    const bool existId(unsigned int id);
    unsigned int modifyId(unsigned int id);

    // Debugger
    void createTask(std::vector<TCB*>& pTasks, unsigned int idTask);
    void chosenMode(const unsigned int& currentIdTask, const unsigned int& globalClock, const unsigned int& currentTaskQuantum);
    void showMinimumInfo(const unsigned int& currentIdTask, const unsigned int& globalClock, const unsigned int& currentTaskQuantum);
    void showAllTasks(const unsigned int& currentIdTask, const unsigned int& globalClock, const unsigned int& currentTaskQuantum);
    void showReadyTasks(const unsigned int& currentIdTask, const unsigned int& globalClock, const unsigned int& currentTaskQuantum);
};