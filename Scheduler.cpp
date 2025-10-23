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

std::vector<TCB>& Scheduler::getTasks()
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
    switch (algorithmChosen)
    {
    case Algorithm::FIFO:
        // Sem necessidade de ordenacao, pois no simulator ja vem ordenado por ordem de chegada
        readyTasks.push_back(task);
        break;
    
    case Algorithm::SRTF:
        readyTasks.push_back(task);
        // Ordena por menor tempo restante
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB& a, const TCB& b){return a.getRemainingTime() < b.getRemainingTime();});
        break;
    
    case Algorithm::PRIOp:
        readyTasks.push_back(task);
        // Ordena por maior prioridade
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB& a, const TCB& b){return a.getPriority() > b.getPriority();});
        break;
    }
    
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
    if(readyTasks.empty()) return nullptr;
    
    return &(readyTasks[0]);
}
