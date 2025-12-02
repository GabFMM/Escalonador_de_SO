#pragma once

class MutexAction{
public:
    enum class Type {Lock, Unlock};

private:
    // Obs: novo atributo -> mudar sobrecarregador do operador != e =
    
    Type type;

    unsigned int id;
    unsigned int time; // tempo relativo a tarefa

public:
    MutexAction();
    ~MutexAction();

    void setType(Type type);
    void setId(unsigned int id);
    void setTime(unsigned int time);

    Type getType() const;
    unsigned int getId() const;
    unsigned int getTime() const;

    bool operator!=(const MutexAction& other) const;
    MutexAction& operator=(const MutexAction& other);
};