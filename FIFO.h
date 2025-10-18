#pragma once

#include "Scheduler.h"

class FIFO : public Scheduler {
private:
public:

    TCB getNextTask();
};