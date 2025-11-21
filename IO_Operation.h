#pragma once

class IO_Operation{
private:
    unsigned int initialTime;
    unsigned int duration;
    long long remainingTime;

public:
    IO_Operation();
    ~IO_Operation();

    void setInitialTime(unsigned int initialTime);
    void setDuration(unsigned int duration);
    void setRemainingTime(long long remaingTime);

    unsigned int getInitialTime() const;
    unsigned int getDuration() const;
    long long getRemainingTime() const;
};