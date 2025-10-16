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

unsigned int Scheduler::getSumDurationTasls()
{
    unsigned int sum = 0;

    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        sum += tasks[i].getDuration();

    return sum;
}
