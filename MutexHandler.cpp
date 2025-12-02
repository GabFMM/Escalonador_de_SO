#include "MutexHandler.h"

MutexHandler::MutexHandler(Scheduler* s) :
scheduler(s),
mutexes(0)
{
}

MutexHandler::~MutexHandler()
{
    // a destruicao do objeto Scheduler fica com o Simulator
    scheduler = nullptr;

    mutexes.clear();
}

/*
    retorna:
        true, se a tarefa NAO entrou na fila de espera do mutex
        false, caso contrario
*/
const bool MutexHandler::lock(unsigned int mutexId, TCB *task)
{
    // procura o mutex
    size_t i;
    size_t tam = mutexes.size();
    for(i = 0; i < tam && mutexes[i].getId() != mutexId; i++);

    // verifica se nao foi encontrado o mutex
    // nao deve acontecer
    if(i == tam) return true; 

    // se o mutex esta livre para ser usado
    if(!mutexes[i].getLocked()){
        // passa o id de quem esta usando o mutex
        mutexes[i].setOwnerTask(task);

        // seta o mutex como ocupado
        mutexes[i].setLocked(true);

        return true;
    }
    // senao o mutex esta ocupado por outra tarefa
    else{
        // coloca a tarefa na fila de espera
        mutexes[i].addWaitingTask(task);

        // muda o estado da tarefa
        task->setState(TCB::State::Suspended);

        // retira a tarefa da fila de prontas do escalonador
        scheduler->removeTask(task->getId());

        return false;
    }
}

/*
    mutexId tem que existir em mutexes

    retorna true, se uma tarefa na fila de suspensas do mutex foi adicionada no escalonador
    return false, caso contrario
*/
const bool MutexHandler::unlock(unsigned int mutexId)
{
    // procura o mutex
    size_t i;
    for(i = 0; mutexes[i].getId() != mutexId; i++);

    // verifica se o mutex nao esta ocupado (nao deve ocorrer)
    if(!mutexes[i].getLocked()) return false;

    // se nao tiver tarefas na fila de espera
    if(mutexes[i].isWaitingTasksEmpty()){
        // seta como livre o mutex
        mutexes[i].setLocked(false);
        mutexes[i].setOwnerTask(nullptr);

        return false;
    }
    else{
        // atualiza a nova tarefa dona do mutex
        TCB* task = mutexes[i].getFirstWaitingTask();
        mutexes[i].setOwnerTask(task);

        // reinsere a tarefa na fila de prontas do escalonador
        scheduler->addTask(task);

        // atualiza a fila de espera do mutex
        mutexes[i].removeFirstWaitingTask();

        return true;
    }
}

void MutexHandler::createMutexes(const std::vector<TCB *> &tasks)
{
    // para evitar incluir ID's, de mutex iguais, 
    // usados por uma ou mais tarefas diferentes, 
    // eh usada a tabela hash
    // nao ha importancia em ordenar os ID's
    // ja que os mutexes sao ordenados por tempo pelos mutexesAction de cada tarefa
    std::unordered_set<unsigned int> setId;

    size_t tasksTam = tasks.size();
    // percorre cada tarefa
    for(size_t i = 0; i < tasksTam; i++){
        std::vector<unsigned int> v = tasks[i]->getMutexesActionId();

        size_t vTam = v.size();
        // para cada id de mutexAction da tarefa, insere na tabela hash
        // o insert apenas aceita valores unicos
        for(size_t j = 0; j < vTam; j++)
            setId.insert(v[j]);
    }

    // para cada id na tabela hash, cria um objeto Mutex correspondente
    std::unordered_set<unsigned int>::iterator it;
    for(it = setId.begin(); it != setId.end(); it++){
        Mutex m;
        m.setId(*it);

        mutexes.push_back(m);
    }
}

/*
    metodo responsavel por verificar se a tarefa "executada" chama alguma acao de mutex
    
    retorna true, se a tarefa foi suspensa
    retorna false, caso contrario
*/
const bool MutexHandler::canAnyMutexActionOccur(TCB *currentTask)
{
    std::vector<MutexAction> v = currentTask->getMutexesAction();
    size_t tam = v.size();

    // calcula o tempo de execucao da tarefa
    // sem risco de ser negativo
    unsigned int taskExecutedTime = currentTask->getDuration() - currentTask->getRemainingTime();

    for(size_t i = 0; i < tam; i++){
        if(v[i].getTime() == taskExecutedTime){
            // verifica o tipo
            if(v[i].getType() == MutexAction::Type::Lock){
                return !lock(v[i].getId(), currentTask);
            }
            else if(v[i].getType() == MutexAction::Type::Unlock){
                return unlock(v[i].getId());
            }
        }
    }
    
    return false;
}

std::vector<unsigned int> MutexHandler::getSuspendedTasksId()
{
    std::vector<unsigned int> ids;

    size_t mutexesTam = mutexes.size();
    for(size_t i = 0; i < mutexesTam; i++){
        std::queue<TCB*> waitingTasks = mutexes[i].getWaitingTasks();

        while(!waitingTasks.empty()){
            ids.push_back(waitingTasks.front()->getId());
            waitingTasks.pop();
        }
    }

    return ids;
}

std::vector<std::variant<int, std::string>> MutexHandler::getSuspendedTasksColor()
{
    std::vector<std::variant<int, std::string>> res;

    size_t mutexesTam = mutexes.size();
    for(size_t i = 0; i < mutexesTam; i++){
        std::queue<TCB*> waitingTasks = mutexes[i].getWaitingTasks();

        while(!waitingTasks.empty()){
            TCB* task = waitingTasks.front();
            if(task->getColor() != 0)
                res.push_back(task->getColor());
            else
                res.push_back(task->getStrColor());

            waitingTasks.pop();
        }
    }

    return res;
}
