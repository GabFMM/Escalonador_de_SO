#include "MutexAction.h"

MutexAction::MutexAction() :
type(MutexAction::Type::Lock),
id(0),
time(0)
{
}

MutexAction::~MutexAction()
{
    type = MutexAction::Type::Lock;
    id = 0;
    time = 0;
}

void MutexAction::setType(Type type)
{
    this->type = type;
}

void MutexAction::setId(unsigned int id)
{
    this->id = id;
}

void MutexAction::setTime(unsigned int time)
{
    this->time = time;
}

MutexAction::Type MutexAction::getType() const
{
    return type;
}

unsigned int MutexAction::getId() const
{
    return id;
}

unsigned int MutexAction::getTime() const
{
    return time;
}

bool MutexAction::operator!=(const MutexAction &other) const
{
    if(type != other.getType())
        return true;
    
    if(id != other.getId())
        return true;

    if(time != other.getTime())
        return true;

    return false;
}

MutexAction &MutexAction::operator=(const MutexAction &other)
{
    // evita auto-atribuicao
    if (this == &other) return *this;

    type = other.getType();
    id = other.getId();
    time = other.getTime();

    return *this;
}
