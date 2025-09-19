#pragma once

class TCB{
private:
    int id;
    int color;
    unsigned int entryTime;
    unsigned int duration;
    unsigned int priority;
    unsigned int remainingTime;
    unsigned int endTime;

public:
    TCB();
    ~TCB();

    // Setters
    void setId(int id);
    void setColor(int color);
    void setEntryTime(unsigned int entryTime);
    void setDuration(unsigned int duration);
    void setPriority(unsigned int priority);
    void setRemainingTime(unsigned int remainingTime);
    void setEndTime(unsigned int endTime);

    // Getters
    int getId() const;
    int getColor() const;
    unsigned int getEntryTime() const;
    unsigned int getDuration() const;
    unsigned int getPriority() const;
    unsigned int getRemainingTime() const;
    unsigned int getEndTime() const;
};