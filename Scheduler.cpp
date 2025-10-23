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

std::vector<int> Scheduler::getIdTasks()
{
    std::vector<int> idTasks(readyTasks.size(), 0);

    std::size_t tam = readyTasks.size();
    for(std::size_t i = 0; i < tam; i++)
        idTasks[i] = readyTasks[i].getId();

    return idTasks;
}

void Scheduler::addTask(TCB task)
{
    readyTasks.push_back(task);
}

void Scheduler::removeTask(unsigned int idTask)
{
    std::vector<TCB>::iterator it;
    for(it = readyTasks.begin(); it->getId() != idTask; it++);
    
    readyTasks.erase(it);
}

const bool Scheduler::existTask() const
{
    return readyTasks.size();
}

TCB* Scheduler::getNextTask()
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
        return getNextTaskPRIOp();
        break;
    }

    return nullptr;
}

TCB* Scheduler::getNextTaskFIFO()
{
    if(readyTasks.empty()) return nullptr;
    
    return &(readyTasks[0]);
}

TCB* Scheduler::getNextTaskSRTF()
{
    return nullptr;
}

TCB* Scheduler::getNextTaskPRIOp()
{
    return nullptr;
}
