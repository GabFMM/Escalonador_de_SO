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
    for(auto &op : IO_operations){
        if(op.getInitialTime() == initialTime){
            op.setRemainingTime(newRemainingTime);
            return;
        }
    }
}

std::vector<MutexAction> TCB::getMutexesAction() const
{
    return mutexesAction;
}

// retorna os mutexes action com o mesmo id passado por parametro
std::vector<MutexAction> TCB::getMutexesAction(unsigned int id) const
{
    std::vector<MutexAction> v;

    size_t tam = mutexesAction.size();
    for(size_t i = 0; i < tam; i++)
        if(mutexesAction[i].getId() == id)
            v.push_back(mutexesAction[i]);

    return v;
}

std::vector<unsigned int> TCB::getMutexesActionId() const
{
    std::vector<unsigned int> v(mutexesAction.size(), 0);

    size_t tam = mutexesAction.size();
    for(size_t i = 0; i < tam; i++)
        v[i] = mutexesAction[i].getId();

    return v;
}

void TCB::addMutexAction(MutexAction m)
{
    mutexesAction.push_back(m);
}

// o m tem que existir na tarefa
void TCB::removeMutexAction(MutexAction m)
{
    std::vector<MutexAction>::iterator it;
    for(it = mutexesAction.begin(); (*it) != m; it++);

    mutexesAction.erase(it);
}

// usado em loadArquive de simulator.h
// caso retorne falso, a tarefa será apagada em loadArquive
const bool TCB::validateMutexesAction(std::string &error)
{
    // ordena por tempo os mutexesAction
    // util para verificar se nao houve um mutex unlock antes de um lock
    // util para organizar a ordem de leitura do arquivo de configuracao
    std::sort(mutexesAction.begin(), mutexesAction.end(),
          [](const MutexAction& a, const MutexAction& b){
              return a.getTime() < b.getTime();
          });

    // tabela hash dos id's dos mutexes do tipo lock
    std::unordered_set<unsigned int> held;

    for (const auto &act : mutexesAction) {

        // se o mutexAction for do tipo Lock
        if (act.getType() == MutexAction::Type::Lock) {
            // verifica se nao ha dois mutexes lock com o mesmo id
            if (held.count(act.getId())) {
                error = "Mutex " + std::to_string(act.getId()) +
                        " required twice without being released.";
                return false;
            }

            // insere o id do mutex lock na tabela hash
            held.insert(act.getId());
        }

        // se o mutexAction for do tipo Unlock
        else if (act.getType() == MutexAction::Type::Unlock) {
            // verifica se o mutexAction do tipo Unlock ocorre 
            // sem um mutexAction correspondente (mesmo id) do tipo Lock
            if (!held.count(act.getId())) {
                error = "Mutex " + std::to_string(act.getId()) +
                        " released without being purchased.";
                return false;
            }

            // remove o id do mutex lock na tabela hash
            held.erase(act.getId());
        }
    }

    // verifica se algum mutex do tipo Lock nao foi liberado
    // isto eh, nao tem um mutex do tipo Unlock com o mesmo id
    if (!held.empty()) {
        error = "Task ended with unreleased mutexes.";
        return false;
    }

    return true;
}

const bool TCB::canAddMutexesAction(unsigned int id, unsigned int lockTime, unsigned int unlockTime)
{
    bool locked = false;       // indica se o mutex esta atualmente bloqueado
    unsigned int lastTime = 0; // tempo da última ação deste mutex action ID

    for (const auto &act : mutexesAction)
    {
        if (act.getId() != id)
            continue;

        // Ações devem ser avaliadas por ordem cronológica
        if (act.getTime() < lastTime)
            lastTime = act.getTime();

        if (act.getType() == MutexAction::Type::Lock)
        {
            if (locked)
                return false; // dois locks seguidos
            locked = true;

            // novo lock não pode vir depois de um MU com tempo menor
            if (lockTime < act.getTime() && locked)
                return false;
        }
        else // Unlock
        {
            if (!locked)
                return false; // unlock sem lock anterior
            locked = false;

            // novo unlock não pode vir antes desse unlock
            if (unlockTime < act.getTime())
                return false;
        }
    }

    // Também evitar lockTime > unlockTime
    if (unlockTime <= lockTime)
        return false;

    return true;
}

const bool TCB::existMutexActionId(unsigned int id)
{
    size_t tam = mutexesAction.size();
    for(size_t i = 0; i < tam; i++)
        if(mutexesAction[i].getId() == id)
            return true;

    return false;
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
        mutexesAction = t.mutexesAction;
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
        mutexesAction = t.mutexesAction;
    }
    return *this; // retorna o objeto atual (por referência)
}
