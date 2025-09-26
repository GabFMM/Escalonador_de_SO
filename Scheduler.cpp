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

void Scheduler::addTask(TCB task)
{
    tasks.push_back(task);
}
