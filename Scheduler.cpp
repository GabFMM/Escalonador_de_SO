#include "Scheduler.h"

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
    tasks.clear();
}

void Scheduler::setTasks(std::vector<TCB> t)
{
    tasks = t;
}

std::vector<TCB> Scheduler::getTasks()
{
    return tasks;
}

void Scheduler::addTask(TCB task)
{
    tasks.push_back(task);
}

const bool Scheduler::existTask() const
{
    return tasks.size();
}
