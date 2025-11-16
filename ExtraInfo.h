#pragma once

#include <string>

class ExtraInfo{
private:
    std::string algorithmScheduler;
    unsigned int quantum;
    unsigned int alpha;

public:
    ExtraInfo();
    ~ExtraInfo();

    void setAlgorithmScheduler(std::string algorithm);
    void setQuantum(unsigned int quantum);
    void setAlpha(unsigned int alpha);

    std::string getAlgorithmScheduler() const;
    unsigned int getQuantum() const;
    unsigned int getAlpha() const;
};