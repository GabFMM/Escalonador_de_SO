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

public:
    Scheduler();
    ~Scheduler();

    void setAlgorithm(Algorithm algo);

    // manipulacao de readyTasks

    // getters
    std::vector<TCB*>& getTasks();
    std::vector<unsigned int> getIdTasks();
    std::vector<unsigned int> getReadyTasksId();
    std::vector<std::variant<int, std::string>> getReadyTasksColor();
    TCB* getNextTask();
    // setters
    void setTasks(std::vector<TCB*> t);

    void addTask(TCB* task, const unsigned int& alpha = 0);
    void removeTask(unsigned int idTask);
    const bool existTask() const;
    void taskQuantumEnded();
    void updateDynamicPriorityTasks(unsigned int alpha);
};