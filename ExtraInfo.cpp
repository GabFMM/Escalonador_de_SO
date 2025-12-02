#include "ExtraInfo.h"

ExtraInfo::ExtraInfo() : quantum(0), algorithmScheduler("")
{
    algorithmScheduler.clear();
}

ExtraInfo::~ExtraInfo()
{
    quantum = 0;
    algorithmScheduler.clear();
}

void ExtraInfo::setAlgorithmScheduler(std::string algorithm)
{
    algorithmScheduler.clear(); // util se o algoritmo for editado
    algorithmScheduler.append(algorithm);
}

void ExtraInfo::setQuantum(unsigned int quantum)
{
    this->quantum = quantum;
}

std::string ExtraInfo::getAlgorithmScheduler() const
{
    return algorithmScheduler;
}

unsigned int ExtraInfo::getQuantum() const
{
    return quantum;
}
