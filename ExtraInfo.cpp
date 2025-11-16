#include "ExtraInfo.h"

ExtraInfo::ExtraInfo() : quantum(0), algorithmScheduler(""), alpha(0)
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

void ExtraInfo::setAlpha(unsigned int alpha)
{
    this->alpha = alpha;
}

std::string ExtraInfo::getAlgorithmScheduler() const
{
    return algorithmScheduler;
}

unsigned int ExtraInfo::getQuantum() const
{
    return quantum;
}

unsigned int ExtraInfo::getAlpha() const
{
    return alpha;
}
