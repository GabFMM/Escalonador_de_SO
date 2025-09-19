#include "TCB.h"

TCB::TCB() :
id(0),
color(0),
entryTime(0),
duration(0),
priority(0),
remainingTime(0),
endTime(0){

}

TCB::~TCB(){
    id = 0;
    color = 0;
    entryTime = 0;
    duration = 0;
    priority = 0;
    remainingTime = 0;
    endTime = 0;
}

// Setters
void TCB::setId(int id) {
    this->id = id;
}

void TCB::setColor(int color) {
    this->color = color;
}

void TCB::setEntryTime(unsigned int entryTime) {
    this->entryTime = entryTime;
}

void TCB::setDuration(unsigned int duration) {
    this->duration = duration;
}

void TCB::setPriority(unsigned int priority) {
    this->priority = priority;
}

void TCB::setRemainingTime(unsigned int remainingTime) {
    this->remainingTime = remainingTime;
}

void TCB::setEndTime(unsigned int endTime) {
    this->endTime = endTime;
}

// Getters
int TCB::getId() const {
    return id;
}

int TCB::getColor() const {
    return color;
}

unsigned int TCB::getEntryTime() const {
    return entryTime;
}

unsigned int TCB::getDuration() const {
    return duration;
}

unsigned int TCB::getPriority() const {
    return priority;
}

unsigned int TCB::getRemainingTime() const {
    return remainingTime;
}

unsigned int TCB::getEndTime() const {
    return endTime;
}
