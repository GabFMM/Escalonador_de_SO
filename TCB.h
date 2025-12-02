#pragma once

#include <limits>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <unordered_set>

#include "IO_Operation.h"
#include "MutexAction.h"

class TCB{
public:
    enum class State {New, Ready, Running, Suspended, Finished};

private:
    // Obs: novo atributo -> mudar sobrecarga do operador= e metodo copyTCB

    State state;

    unsigned int id;
    int color;            // formato antigo
    std::string strColor; // formato hexadecimal para RGB
    unsigned int entryTime;
    unsigned int duration;
    unsigned int staticPriority;
    unsigned int dynamicPriority;
    long long remainingTime; // talvez trocar long long por unsigned int
    unsigned int endTime;

    std::vector<IO_Operation> IO_operations;
    std::vector<MutexAction> mutexesAction;

public:
    TCB();
    ~TCB();

    // Setters
    void setId(unsigned int id);
    void setColor(int color);
    void setColor(std::string str);
    void setEntryTime(unsigned int entryTime);
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
    unsigned int getDuration() const;
    unsigned int getStaticPriority() const;
    unsigned int getDynamicPriority() const;
    long long getRemainingTime() const;
    unsigned int getEndTime() const;
    State getState() const;

    // IO_Operations
    std::vector<IO_Operation> getIO_operations() const;
    IO_Operation getIO_operation(const unsigned int& initialTime);
    IO_Operation getFirstIO_Operation();
    const bool addIO_operation(std::pair<unsigned int, unsigned int> op);
    void removeIO_operation(const unsigned int& initialTime);
    void setIO_OperationRemainingTime(const unsigned int& initialTime, const long long& newRemainingTime);

    // Mutexes
    std::vector<MutexAction> getMutexesAction() const;
    std::vector<MutexAction> getMutexesAction(unsigned int id) const;
    std::vector<unsigned int> getMutexesActionId() const;
    void addMutexAction(MutexAction m);
    void removeMutexAction(MutexAction m);
    const bool validateMutexesAction(std::string& error);
    const bool canAddMutexesAction(unsigned int id, unsigned int lockTime, unsigned int unlockTime);
    const bool existMutexActionId(unsigned int id);

    void copyTCB(const TCB& t);

    // Sobrecarga de operadores
    TCB& operator=(const TCB& t);
};