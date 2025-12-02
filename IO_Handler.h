#pragma once

#include <vector>
#include <variant>
#include <string>

#include "TCB.h"
#include "Scheduler.h"

class IO_Handler{
private:
    Scheduler* scheduler;

    std::vector<TCB*> suspendedTasks;

public:
    IO_Handler(Scheduler* s);
    ~IO_Handler();

    // getters
    std::vector<TCB*> getSuspendedTasks() const;
    std::vector<unsigned int> getSuspendedTasksId() const;
    std::vector<std::variant<int, std::string>> getSuspendedTasksColor() const;

    void addTask(TCB* task);
    const bool updateSuspendedTasks(const unsigned int& deltaTime);

    const bool canAnyIO_OperationBegin(TCB* currentTask);
};