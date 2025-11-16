#pragma once

#include <limits>
#include <string>

class TCB{
public:
    enum class State {New, Ready, Running, Finished};

private:
    unsigned int id;
    int color;            // formato antigo
    std::string strColor; // formato hexadecimal para RGB
    unsigned int entryTime;
    unsigned int lastUsedTime;
    unsigned int duration;
    unsigned int staticPriority;
    unsigned int dynamicPriority;
    long long remainingTime;
    unsigned int endTime;
    // Obs: novo atributo -> mudar sobrecarga do operador= e metodo copyTCB

    State state;

public:
    TCB();
    ~TCB();

    // Setters
    void setId(unsigned int id);
    void setColor(int color);
    void setColor(std::string str);
    void setEntryTime(unsigned int entryTime);
    void setLastUsedTime(unsigned int lastUsedTime);
    void setDuration(unsigned int duration);
    void setStaticPriority(unsigned int priority);
    void setDynamicPriority(unsigned int priority);
    void setRemainingTime(long long remainingTime);
    void setEndTime(unsigned int endTime);
    void setState(State state);

    // Getters
    unsigned int getId() const;
    int getColor() const;
    std::string getStrColor() const;
    unsigned int getEntryTime() const;
    unsigned int getLastUsedTime() const;
    unsigned int getDuration() const;
    unsigned int getStaticPriority() const;
    unsigned int getDynamicPriority() const;
    unsigned int getRemainingTime() const;
    unsigned int getEndTime() const;
    State getState() const;

    void copyTCB(const TCB& t);

    // Sobrecarga de operadores
    TCB& operator=(const TCB& t);
};