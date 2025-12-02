#pragma once

#include <iostream>
#include <vector> // TO-DO: mudar para list para melhor desempenho em removeTask
#include <algorithm>
#include <variant>

#include "TCB.h"

class Scheduler{
public:
    enum class Algorithm {FIFO, SRTF, PRIOp, PRIOPEnv};

private:
    std::vector<TCB*> readyTasks;

    Algorithm algorithmChosen;
    unsigned int alpha; // usado no PRIOPEnv

public:
    Scheduler();
    ~Scheduler();

    void setAlgorithm(Algorithm algo);
    void setAlpha(unsigned int a);
    Algorithm getAlgorithm() const;
    unsigned int getAlpha() const;

    // manipulacao de readyTasks

    // getters
    std::vector<TCB*>& getTasks();
    std::vector<unsigned int> getIdTasks();
    std::vector<unsigned int> getReadyTasksId();
    std::vector<std::variant<int, std::string>> getReadyTasksColor();
    TCB* getNextTask();
    // setters
    void setTasks(std::vector<TCB*> t);

    void addTask(TCB* task);
    void addExecutedTask(TCB* task);
    void removeTask(unsigned int idTask);
    const bool existTask() const;
    void taskQuantumEnded(const unsigned int& quantum);
    void updateDynamicPriorityTasks();
    void sortReadyTasks();
};