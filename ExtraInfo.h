#pragma once

#include <string>

class ExtraInfo{
private:
    std::string algorithmScheduler;
    unsigned int quantum;

public:
    ExtraInfo();
    ~ExtraInfo();

    void setAlgorithmScheduler(std::string algorithm);
    void setQuantum(unsigned int quantum);

    std::string getAlgorithmScheduler() const;
    unsigned int getQuantum() const;
};