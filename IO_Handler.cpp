#include "IO_Handler.h"

IO_Handler::IO_Handler(Scheduler* s) :
scheduler(s),
suspendedTasks(0)
{
}

IO_Handler::~IO_Handler()
{
    // a destruicao de scheduler fica por conta de Simulator
    scheduler = nullptr; 
    suspendedTasks.clear();
}

std::vector<TCB *> IO_Handler::getSuspendedTasks() const
{
    return suspendedTasks;
}

// adiciona a tarefa na fila de suspensas
void IO_Handler::addTask(TCB *task)
{
    if(task != nullptr){
        task->setState(TCB::State::Suspended);
        suspendedTasks.push_back(task);
    }
}

/*
    metodo responsavel por atualizar as operacoes I/O de cada tarefa
    se o I/O acabar, a tarefa dona eh removida da fila de espera
    e eh inserida na fila de prontas do escalonador

    retorna true, se a tarefa for adicionada no escalonador
    retorna false, caso contrario
*/
const bool IO_Handler::updateSuspendedTasks(const unsigned int &deltaTime)
{
    bool exit = false;

    auto it = suspendedTasks.begin();

    while (it != suspendedTasks.end()) {

        TCB* tcb = *it;

        // pega uma única vez
        IO_Operation op = tcb->getFirstIO_Operation();
        unsigned int initialTime = op.getInitialTime();

        // atualiza
        tcb->setIO_OperationRemainingTime(initialTime,
            op.getRemainingTime() - deltaTime);

        // pega novamente só depois de atualizar
        op = tcb->getFirstIO_Operation();

        if (op.getRemainingTime() <= 0) {

            // adiciona a tarefa na fila de prontas do escalonador
            scheduler->addTask(tcb);
            exit = true;

            tcb->removeIO_operation(initialTime);

            // remove a tarefa na fila de suspensas do I/O
            it = suspendedTasks.erase(it); // move o iterador
        }
        else {
            it++;
        }
    }

    return exit;
}

std::vector<unsigned int> IO_Handler::getSuspendedTasksId() const
{
    std::vector<unsigned int> ids(suspendedTasks.size());

    size_t tam = ids.size();
    for(size_t i = 0; i < tam; i++)
        ids[i] =  suspendedTasks[i]->getId();

    return ids;
}

std::vector<std::variant<int, std::string>> IO_Handler::getSuspendedTasksColor() const
{
    std::vector<std::variant<int, std::string>> colors(suspendedTasks.size());

    size_t tam = colors.size();
    for(size_t i = 0; i < tam; i++)
        if(suspendedTasks[i]->getColor() != 0)
            colors[i] = suspendedTasks[i]->getColor();
        else
            colors[i] = suspendedTasks[i]->getStrColor();

    return colors;
}

/*
    metodo responsavel por verificar se alguma operacao I/O pode ocorrer na tarefa executada
    caso positivo, a tarefa eh incluida na fila de suspensas

    retorna true, se a tarefa entrou na fila de suspensas
    retorna false, caso contrario
*/
const bool IO_Handler::canAnyIO_OperationBegin(TCB *currentTask)
{
    std::vector<IO_Operation> ops = currentTask->getIO_operations();
    size_t tam = ops.size();

    // calcula o tempo de execucao da tarefa
    // sem risco de ser negativo
    unsigned int taskExecutedTime = currentTask->getDuration() - currentTask->getRemainingTime();

    // as operacoes estao ordenadas por tempo
    // entao nao vai ser tao custoso essa procura
    for(size_t i = 0; i < tam; i++){
        if(ops[i].getInitialTime() == taskExecutedTime){
            // remove a tarefa na fila de prontas do escalonador
            scheduler->removeTask(currentTask->getId());

            // insere a tarefa na fila de espera do I/O
            addTask(currentTask);
            return true;
        }
    }

    return false;
}
