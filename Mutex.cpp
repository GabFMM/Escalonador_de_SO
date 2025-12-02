#include "Mutex.h"

Mutex::Mutex() :
locked(false),
ownerTask(nullptr),
waitingTasks()
{
}

Mutex::~Mutex()
{
    locked = true;
    ownerTask = nullptr;
    
    // limpa a fila
    while(!waitingTasks.empty())
        waitingTasks.pop();
}

void Mutex::setLocked(bool state)
{
    locked = state;
}

void Mutex::setId(unsigned int id)
{
    this->id = id;
}

void Mutex::setOwnerTask(TCB* task)
{
    ownerTask = task;
}

const bool Mutex::getLocked() const
{
    return locked;
}

unsigned int Mutex::getId() const
{
    return id;
}

TCB* Mutex::getOwnerTask() const
{
    return ownerTask;
}

std::queue<TCB *> Mutex::getWaitingTasks() const
{
    return waitingTasks;
}

const bool Mutex::isWaitingTasksEmpty() const
{
    return waitingTasks.empty();
}

void Mutex::addWaitingTask(TCB *task)
{
    waitingTasks.push(task);
}

TCB *Mutex::getFirstWaitingTask() const
{
    return waitingTasks.front();
}

void Mutex::removeFirstWaitingTask()
{
    // o pop apenas remove o ponteiro da fila
    // nao deleta o objeto
    // o intuito eh esse mesmo
    waitingTasks.pop();
}
