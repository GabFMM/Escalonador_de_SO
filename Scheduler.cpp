#include "Scheduler.h"

Scheduler::Scheduler() : 
readyTasks(),
alpha(0)
{
}

Scheduler::~Scheduler()
{
    readyTasks.clear();
    alpha = 0;
}

void Scheduler::setAlgorithm(Algorithm algo)
{
    algorithmChosen = algo;
}

void Scheduler::setAlpha(unsigned int a)
{
    alpha = a;
}

Scheduler::Algorithm Scheduler::getAlgorithm() const
{
    return algorithmChosen;
}

unsigned int Scheduler::getAlpha() const
{
    return alpha;
}

void Scheduler::setTasks(std::vector<TCB*> t)
{
    readyTasks = t;
}

std::vector<TCB*>& Scheduler::getTasks()
{
    return readyTasks;
}

std::vector<unsigned int> Scheduler::getIdTasks()
{
    std::vector<unsigned int> idTasks(readyTasks.size(), 0);

    std::size_t tam = readyTasks.size();
    for(std::size_t i = 0; i < tam; i++)
        idTasks[i] = readyTasks[i]->getId();

    return idTasks;
}

std::vector<unsigned int> Scheduler::getReadyTasksId()
{
    std::vector<unsigned int> v(readyTasks.size()); 

    size_t tam = v.size();
    for(size_t i = 0; i < tam; i++)
        v[i] = readyTasks[i]->getId();

    return v;
}

std::vector<std::variant<int, std::string>> Scheduler::getReadyTasksColor()
{

    // o variant pode assumir um dos dois tipos, a depender de qual foi armazenado
    std::vector<std::variant<int, std::string>> v(readyTasks.size());

    size_t tam = v.size();
    for(size_t i = 0; i < tam; i++)
        if(readyTasks[i]->getColor() != 0)
            v[i] = std::variant<int, std::string>{readyTasks[i]->getColor()};
        else
            v[i] = std::variant<int, std::string>{readyTasks[i]->getStrColor()};

    return v;
}

void Scheduler::addTask(TCB* task)
{
    if (task->getState() == TCB::State::Finished) return;

    // atualiza o estado da tarefa para pronta
    task->setState(TCB::State::Ready);

    // adiciona a tarefa no fim da fila
    readyTasks.push_back(task);

    // reordena a fila de prontas
    sortReadyTasks();
}

void Scheduler::addExecutedTask(TCB *task)
{
    if (task->getState() != TCB::State::Running ) return;

    // atualiza o estado da tarefa para pronta
    task->setState(TCB::State::Ready);

    // adiciona a tarefa no inicio da fila
    readyTasks.insert(readyTasks.begin(), task);

    // reordena a fila de prontas
    sortReadyTasks();
}

// Apenas apaga o ponteiro que aponta para TCB que possui o idTask
void Scheduler::removeTask(unsigned int idTask)
{
    std::vector<TCB*>::iterator it;
    for(it = readyTasks.begin(); it != readyTasks.end(); it++){
        if((*it)->getId() == idTask){
            readyTasks.erase(it);
            return;
        }
    }
}

const bool Scheduler::existTask() const
{
    return readyTasks.size();
}

void Scheduler::taskQuantumEnded(const unsigned int& quantum)
{
    // caso especial do RR
    if(algorithmChosen == Algorithm::FIFO && quantum > 0){
        // rotaciona a fila
        if (!readyTasks.empty()) {
            // move o primeiro elemento para o final
            TCB* first = readyTasks.front();
            readyTasks.erase(readyTasks.begin());
            readyTasks.push_back(first);
        }
    }
}

void Scheduler::updateDynamicPriorityTasks()
{
    size_t tam = readyTasks.size();
    for(size_t i = 0; i < tam; i++)
        readyTasks[i]->setDynamicPriority(readyTasks[i]->getDynamicPriority() + alpha);
}

void Scheduler::sortReadyTasks()
{
    switch (algorithmChosen)
    {
    case Algorithm::FIFO:
        // sem ordenacao, pois a logica eh seguir uma fila
        break;
    
    case Algorithm::SRTF:
        // Ordena por menor tempo restante
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB* a, const TCB* b){return a->getRemainingTime() < b->getRemainingTime();});
        break;
    
    case Algorithm::PRIOp:
        // Ordena por maior prioridade estatica
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB* a, const TCB* b){return a->getStaticPriority() > b->getStaticPriority();});
        break;
    
    case Algorithm::PRIOPEnv:
        updateDynamicPriorityTasks();

        // Ordena por maior prioridade dinamica
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB* a, const TCB* b){return a->getDynamicPriority() > b->getDynamicPriority();});
        break;
    }
}

TCB* Scheduler::getNextTask()
{
    if(readyTasks.empty()) return nullptr;
    
    // a primeira tarefa na fila de prontas sera a nova tarefa "executada"
    TCB* executedTask = readyTasks[0];

    // atualiza o estado da tarefa "executada"
    executedTask->setState(TCB::State::Running);

    // remove a tarefa executada da fila de prontas
    removeTask(executedTask->getId());

    // arruma prioridade dinamica da tarefa executada
    executedTask->setDynamicPriority(executedTask->getStaticPriority());

    return executedTask;
}
