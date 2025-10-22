#include "Scheduler.h"

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
    readyTasks.clear();
}

void Scheduler::setAlgorithm(Algorithm algo)
{
    algorithmChosen = algo;
}

void Scheduler::setTasks(std::vector<TCB> t)
{
    readyTasks = t;
}

std::vector<TCB> Scheduler::getTasks()
{
    return readyTasks;
}

void Scheduler::addTask(TCB task)
{
    readyTasks.push_back(task);
}

void Scheduler::removeTask(unsigned int idTask)
{
    std::vector<TCB>::iterator it;
    for(it = readyTasks.begin(); it->getId() == idTask; it++);
    
    readyTasks.erase(it);
}

const bool Scheduler::existTask() const
{
    return readyTasks.size();
}

TCB Scheduler::getNextTask()
{
    switch (algorithmChosen)
    {
    case Algorithm::FIFO:
        return getNextTaskFIFO();
        break;
    
    case Algorithm::SRTF:
        return getNextTaskSRTF();
        break;
    
    case Algorithm::PRIOp:
        return getNextTaskPrioP();
        break;
    }
}

TCB Scheduler::getNextTaskFIFO()
{
    return readyTasks[0];
}
