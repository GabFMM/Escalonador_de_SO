#pragma once

#include <limits>
#include <string>

class TCB{
private:
    unsigned int id;
    int color;            // formato antigo
    std::string strColor; // formato hexadecimal para RGB
    unsigned int entryTime;
    unsigned int lastUsedTime;
    unsigned int duration;
    unsigned int priority;
    long long remainingTime;
    unsigned int endTime;
    // Obs: novo atributo -> mudar sobrecarga do operador= e metodo copyTCB

public:
    TCB();
    ~TCB();

    // Setters
    void setId(int id);
    void setColor(int color);
    void setColor(std::string str);
    void setEntryTime(unsigned int entryTime);
    void setLastUsedTime(unsigned int lastUsedTime);
    void setDuration(unsigned int duration);
    void setPriority(unsigned int priority);
    void setRemainingTime(long long remainingTime);
    void setEndTime(unsigned int endTime);

    // Getters
    int getId() const;
    int getColor() const;
    std::string getStrColor() const;
    unsigned int getEntryTime() const;
    unsigned int getLastUsedTime() const;
    unsigned int getDuration() const;
    unsigned int getPriority() const;
    unsigned int getRemainingTime() const;
    unsigned int getEndTime() const;

    void copyTCB(const TCB& t);

    // Sobrecarga de operadores
    TCB& operator=(const TCB& t);
};