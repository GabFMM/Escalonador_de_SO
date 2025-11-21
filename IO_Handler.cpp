#include "IO_Handler.h"

IO_Handler::IO_Handler() : suspendedTasks(0)
{
}

IO_Handler::~IO_Handler()
{
    suspendedTasks.clear();
}

std::vector<TCB *> IO_Handler::getSuspendedTasks() const
{
    return suspendedTasks;
}

void IO_Handler::addSuspendedTask(TCB *task)
{
    if(task != nullptr)
        suspendedTasks.push_back(task);
}

void IO_Handler::updateSuspendedTasks(const unsigned int &deltaTime)
{
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

            tcb->setState(TCB::State::New);

            tcb->removeIO_operation(initialTime);

            it = suspendedTasks.erase(it); // move o iterador
        }
        else {
            ++it;
        }
    }
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

const bool IO_Handler::canAnyIO_OperationBegin(const TCB *currentTask)
{
    std::vector<IO_Operation> ops = currentTask->getIO_operations();
    size_t tam = ops.size();

    // calcula o tempo de execucao da tarefa
    // sem risco de ser negativo
    unsigned int taskExecutedTime = currentTask->getDuration() - currentTask->getRemainingTime();

    // as operacoes estao ordenadas por tempo
    // entao nao vai ser tao custoso essa procura
    for(size_t i = 0; i < tam; i++)
        if(ops[i].getInitialTime() == taskExecutedTime)
            return true;
    
    return false;
}
