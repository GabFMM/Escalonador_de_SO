#pragma once

#include <vector>
#include "TCB.h"

class Scheduler{
private:
    std::vector<TCB> tasks;

public:
    Scheduler();
    ~Scheduler();

    void setTasks(std::vector<TCB> t);
    void addTask(TCB task);
};