#include "TCB.h"

TCB::TCB() :
id(0),
color(0),
strColor(6, ' '),
entryTime(0),
duration(0),
staticPriority(0),
dynamicPriority(0),
remainingTime(0),
endTime(std::numeric_limits<unsigned int>::max()),
state(State::New)
{
    
}

TCB::~TCB(){
    id = 0;
    color = 0;
    strColor.clear();
    entryTime = 0;
    duration = 0;
    staticPriority = 0;
    dynamicPriority = 0;
    remainingTime = 0;
    endTime = 0;
    state = State::Finished;
}

// Setters
void TCB::setId(unsigned int id) {
    this->id = id;
}

void TCB::setColor(int color) {
    this->color = color;
}

void TCB::setColor(std::string str)
{
    this->strColor = str;
}

void TCB::setEntryTime(unsigned int entryTime) {
    this->entryTime = entryTime;
}

void TCB::setLastUsedTime(unsigned int lastUsedTime)
{
    this->lastUsedTime = lastUsedTime;
}

void TCB::setDuration(unsigned int duration) {
    this->duration = duration;
    setRemainingTime(duration);
}

void TCB::setStaticPriority(unsigned int priority) {
    this->staticPriority = priority;
    this->dynamicPriority = priority;
}

void TCB::setDynamicPriority(unsigned int priority)
{
    this->dynamicPriority = priority;
}

void TCB::setRemainingTime(long long remainingTime) {
    this->remainingTime = remainingTime;
}

void TCB::setEndTime(unsigned int endTime) {
    this->endTime = endTime;
}

void TCB::setState(State state)
{
    this->state = state;
}

// Getters
unsigned int TCB::getId() const {
    return id;
}

int TCB::getColor() const {
    return color;
}

std::string TCB::getStrColor() const
{
    return strColor;
}

unsigned int TCB::getEntryTime() const {
    return entryTime;
}

unsigned int TCB::getLastUsedTime() const
{
    return lastUsedTime;
}

unsigned int TCB::getDuration() const {
    return duration;
}

unsigned int TCB::getStaticPriority() const {
    return staticPriority;
}

unsigned int TCB::getDynamicPriority() const
{
    return dynamicPriority;
}

unsigned int TCB::getRemainingTime() const {
    return remainingTime;
}

unsigned int TCB::getEndTime() const {
    return endTime;
}

TCB::State TCB::getState() const
{
    return state;
}

// util quando this eh ponteiro
void TCB::copyTCB(const TCB &t)
{
    if (this != &t) { // evita autoatribuição
        
        // Copie aqui os atributos de 't' para 'this'
        id = t.id;
        color = t.color;
        strColor = t.strColor;
        entryTime = t.entryTime;
        duration = t.duration;
        staticPriority = t.staticPriority;
        dynamicPriority = t.dynamicPriority;
        remainingTime = t.remainingTime;
        endTime = t.endTime;
        lastUsedTime = t.lastUsedTime;
        state = t.state;
    }
    return;
}

TCB& TCB::operator=(const TCB &t)
{
    if (this != &t) { // evita autoatribuição
        
        // Copie aqui os atributos de 't' para 'this'
        id = t.id;
        color = t.color;
        strColor = t.strColor;
        entryTime = t.entryTime;
        duration = t.duration;
        staticPriority = t.staticPriority;
        dynamicPriority = t.dynamicPriority;
        remainingTime = t.remainingTime;
        endTime = t.endTime;
        lastUsedTime = t.lastUsedTime;
        state = t.state;
    }
    return *this; // retorna o objeto atual (por referência)
}
