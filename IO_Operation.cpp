#include "IO_Operation.h"

IO_Operation::IO_Operation() :
initialTime(0),
duration(0),
remainingTime(0)
{
    
}

IO_Operation::~IO_Operation()
{
    initialTime = 0;
    duration = 0;
    remainingTime = 0;
}

void IO_Operation::setInitialTime(unsigned int initialTime)
{
    this->initialTime = initialTime;
}

void IO_Operation::setDuration(unsigned int duration)
{
    this->duration = duration;
    setRemainingTime(duration);
}

void IO_Operation::setRemainingTime(long long remaingTime)
{
    this->remainingTime = remaingTime;
}

unsigned int IO_Operation::getInitialTime() const
{
    return initialTime;
}

unsigned int IO_Operation::getDuration() const
{
    return duration;
}

long long IO_Operation::getRemainingTime() const
{
    return remainingTime;
}
