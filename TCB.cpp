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
state(State::New),
IO_operations(0)
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
    IO_operations.clear();
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

const bool TCB::addIO_operation(std::pair<unsigned int, unsigned int> op)
{
    // verifica se a entrada do IO eh maior ou igual que a duracao da tarefa
    if(op.first >= duration) return false;

    // verifica se ja existe uma operacao IO que possui o mesmo tempo de entrada
    auto it = std::lower_bound(
        IO_operations.begin(),
        IO_operations.end(),
        op.first,
        [](const IO_Operation& a, unsigned int value) {
            return a.getInitialTime() < value;
        }
    );

    // verifica se existe operação com o mesmo tempo
    if (it != IO_operations.end() && it->getInitialTime() == op.first)
        return false;

    // cria o objeto I/O
    IO_Operation operation;
    operation.setInitialTime(op.first);
    operation.setDuration(op.second);

    // pode adicionar
    IO_operations.insert(it, operation);

    return true;
}

// Para funcionar, para cada operacao I/O, tem que ter um unico tempo inicial
void TCB::removeIO_operation(const unsigned int &initialTime)
{
    std::vector<IO_Operation>::iterator it;
    for(it = IO_operations.begin(); it != IO_operations.end(); it++){
        if(it->getInitialTime() == initialTime){
            IO_operations.erase(it);
            return;
        }
    }
}

// Para funcionar, para cada operacao I/O, tem que ter um unico tempo inicial
void TCB::setIO_OperationRemainingTime(const unsigned int &initialTime, const long long &newRemainingTime)
{
    for(auto &op : IO_operations)
        if(op.getInitialTime() == initialTime)
            op.setRemainingTime(newRemainingTime);
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

long long TCB::getRemainingTime() const {
    return remainingTime;
}

unsigned int TCB::getEndTime() const {
    return endTime;
}

TCB::State TCB::getState() const
{
    return state;
}

std::vector<IO_Operation> TCB::getIO_operations() const
{
    return IO_operations;
}

// Funciona se ha um unico tempo inicial para cada I/O
// initialTime tem que existir em IO_operations
IO_Operation TCB::getIO_operation(const unsigned int &initialTime)
{
    size_t tam = IO_operations.size();
    size_t i;
    for(i = 0; IO_operations[i].getInitialTime() != initialTime; i++);

    return IO_operations[i];
}

IO_Operation TCB::getFirstIO_Operation()
{
    return IO_operations.at(0);
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
        state = t.state;
        IO_operations = t.IO_operations;
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
        state = t.state;
        IO_operations = t.IO_operations;
    }
    return *this; // retorna o objeto atual (por referência)
}
