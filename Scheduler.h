#pragma once

#include <vector>
#include "TCB.h"

class Scheduler{
protected:
    std::vector<TCB> tasks;

public:
    Scheduler();
    ~Scheduler();

    void setTasks(std::vector<TCB> t);
    std::vector<TCB> getTasks();

    void addTask(TCB task);
    const bool existTask() const;

    virtual TCB getNextTask() = 0;
};