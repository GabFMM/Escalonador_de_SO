#pragma once

#include <vector>
#include <variant>
#include <string>

#include "TCB.h"

class IO_Handler{
private:
    std::vector<TCB*> suspendedTasks;

public:
    IO_Handler();
    ~IO_Handler();

    // getters
    std::vector<TCB*> getSuspendedTasks() const;
    std::vector<unsigned int> getSuspendedTasksId() const;
    std::vector<std::variant<int, std::string>> getSuspendedTasksColor() const;

    void addSuspendedTask(TCB* task);
    void updateSuspendedTasks(const unsigned int& deltaTime);

    const bool canAnyIO_OperationBegin(const TCB* currentTask);
};