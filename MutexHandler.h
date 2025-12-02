#pragma once

#include <vector>
#include <unordered_set>
#include <variant>
#include <string>
#include <queue>

#include "Mutex.h"
#include "TCB.h"
#include "Scheduler.h"

class MutexHandler
{
private:
    Scheduler* scheduler;

    // TO-DO: mudar para unordered set para busca O(1)
    // para isso tem que sobrecarregar o operador de == de Mutex, e
    // criar uma funcao hash para Mutex 
    std::vector<Mutex> mutexes;

public:
    MutexHandler(Scheduler* s);
    ~MutexHandler();

    const bool lock(unsigned int mutexId, TCB* task);
    const bool unlock(unsigned int mutexId);

    void createMutexes(const std::vector<TCB*>& tasks);

    const bool canAnyMutexActionOccur(TCB* currentTask);

    std::vector<unsigned int> getSuspendedTasksId();
    std::vector<std::variant<int, std::string>> getSuspendedTasksColor();
};
