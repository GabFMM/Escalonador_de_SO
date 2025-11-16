#pragma once

#include <iostream>
#include <vector> // TO-DO: mudar para list para melhor desempenho em removeTask
#include <algorithm>

#include "TCB.h"

class Scheduler{
public:
    enum class Algorithm {FIFO, SRTF, PRIOp, PRIOPEnv};

private:
    std::vector<TCB*> readyTasks;

    Algorithm algorithmChosen;

public:
    Scheduler();
    ~Scheduler();

    void setAlgorithm(Algorithm algo);

    // manipulacao de readyTasks
    void setTasks(std::vector<TCB*> t);
    std::vector<TCB*>& getTasks();
    std::vector<unsigned int> getIdTasks();
    TCB* getNextTask();
    void addTask(TCB* task, const unsigned int& alpha = 0);
    void removeTask(unsigned int idTask);
    const bool existTask() const;
    void taskQuantumEnded();
    void updateDynamicPriorityTasks(unsigned int alpha);
};