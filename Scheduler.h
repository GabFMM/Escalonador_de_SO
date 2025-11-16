#pragma once

#include <vector> // TO-DO: mudar para list para melhor desempenho em removeTask
#include <algorithm>

#include "TCB.h"

class Scheduler{
public:
    enum class Algorithm {FIFO, SRTF, PRIOp};

private:
    std::vector<TCB> readyTasks;

    Algorithm algorithmChosen;

public:
    Scheduler();
    ~Scheduler();

    void setAlgorithm(Algorithm algo);

    // manipulacao de readyTasks
    void setTasks(std::vector<TCB> t);
    std::vector<TCB>& getTasks();
    std::vector<int> getIdTasks();
    TCB* getNextTask();
    void addTask(TCB task);
    void removeTask(unsigned int idTask);
    const bool existTask() const;

    void taskQuantumEnded();
};