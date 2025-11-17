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
    // -1, pois a primeira eh sempre a tarefa "executada"
    std::vector<unsigned int> v(readyTasks.size() - 1); 

    size_t tam = v.size();
    for(size_t i = 0; i < tam; i++)
        v[i] = readyTasks[i + 1]->getId();

    return v;
}

// -1, pois a primeira eh sempre a tarefa "executada"
std::vector<std::variant<int, std::string>> Scheduler::getReadyTasksColor()
{
    // o variant pode assumir um dos dois tipos, a depender de qual foi armazenado
    std::vector<std::variant<int, std::string>> v(readyTasks.size() - 1);

    size_t tam = v.size();
    for(size_t i = 0; i < tam; i++)
        if(readyTasks[i + 1]->getColor() != 0)
            v[i] = std::variant<int, std::string>{readyTasks[i + 1]->getColor()};
        else
            v[i] = std::variant<int, std::string>{readyTasks[i + 1]->getStrColor()};

    return v;
}

void Scheduler::addTask(TCB* task, const unsigned int& alpha)
{
    if (task->getState() != TCB::State::New) return;

    task->setState(TCB::State::Ready);

    switch (algorithmChosen)
    {
    case Algorithm::FIFO:
        // Sem necessidade de ordenacao, pois no simulator ja vem ordenado por ordem de chegada
        readyTasks.push_back(task);
        break;
    
    case Algorithm::SRTF:
        readyTasks.push_back(task);
        // Ordena por menor tempo restante
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB* a, const TCB* b){return a->getRemainingTime() < b->getRemainingTime();});
        break;
    
    case Algorithm::PRIOp:
        readyTasks.push_back(task);
        // Ordena por maior prioridade estatica
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB* a, const TCB* b){return a->getStaticPriority() > b->getStaticPriority();});
        break;
    
    case Algorithm::PRIOPEnv:
        updateDynamicPriorityTasks(alpha);

        readyTasks.push_back(task);

        // Ordena por maior prioridade dinamica
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB* a, const TCB* b){return a->getDynamicPriority() > b->getDynamicPriority();});
        break;
    }
}

// Apenas apaga o ponteiro que aponta para TCB que possui o idTask
void Scheduler::removeTask(unsigned int idTask)
{
    std::vector<TCB*>::iterator it;
    for(it = readyTasks.begin(); (*it)->getId() != idTask; it++);
    
    readyTasks.erase(it);
}

const bool Scheduler::existTask() const
{
    return readyTasks.size();
}

// Ocorre quando a tarefa "executada" tem o seu quantum zerado
// Esse metodo deve ser chamado no executeDefault
void Scheduler::taskQuantumEnded()
{
    // Coloca a tarefa "executada" no final do vetor
    TCB* aux = readyTasks[0];
    readyTasks.erase(std::vector<TCB*>::const_iterator(readyTasks.begin()));
    readyTasks.push_back(aux);

    // Reordena o vetor conforme o algoritmo
    switch (algorithmChosen)
    {
    case Algorithm::FIFO:

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
        // Ordena por maior prioridade dinamica
        std::sort(readyTasks.begin(), readyTasks.end(), [](const TCB* a, const TCB* b){return a->getDynamicPriority() > b->getDynamicPriority();});
        break;
    }
}

void Scheduler::updateDynamicPriorityTasks(unsigned int alpha)
{
    size_t tam = readyTasks.size();
    // pula a primeira tarefa, pois ela eh a tarefa "executada"
    // tarefas "executadas" nao tem prioridade dinamica atualizada
    for(size_t i = 1; i < tam; i++)
        readyTasks[i]->setDynamicPriority(readyTasks[i]->getDynamicPriority() + alpha);
}

TCB* Scheduler::getNextTask()
{
    if(readyTasks.empty()) return nullptr;
    
    // arruma prioridade dinamica
    readyTasks[0]->setDynamicPriority(readyTasks[0]->getStaticPriority());

    return readyTasks[0];
}
