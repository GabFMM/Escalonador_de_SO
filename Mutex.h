#pragma once

#include <queue>

#include "TCB.h"

class Mutex
{
private:
    bool locked;
    unsigned int id; // o id do mutex
    TCB* ownerTask; // tarefa que usa o mutex
    std::queue<TCB*> waitingTasks;

public:
    Mutex();
    ~Mutex();

    void setLocked(bool state);
    void setId(unsigned int id);
    void setOwnerTask(TCB* task);

    const bool getLocked() const;
    unsigned int getId() const;
    TCB* getOwnerTask() const;
    std::queue<TCB*> getWaitingTasks() const;

    const bool isWaitingTasksEmpty() const;
    void addWaitingTask(TCB* task);
    TCB* getFirstWaitingTask() const;
    void removeFirstWaitingTask();
};
