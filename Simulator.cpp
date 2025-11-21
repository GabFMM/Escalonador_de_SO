#include "Simulator.h"
#include "Menu.h"

Simulator::Simulator() :
menu(nullptr),
scheduler(nullptr),
io_handler(nullptr),
imageGenerator(nullptr)
{

}

Simulator::~Simulator()
{
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++){
        delete tasks[i];
        tasks[i] = nullptr;
    }
    tasks.clear();

    if(menu != nullptr){
        delete menu;
        menu = nullptr;
    }

    if(scheduler != nullptr){
        delete scheduler;
        scheduler = nullptr;
    }

    if(io_handler != nullptr){
        delete scheduler;
        scheduler = nullptr;
    }

    if(imageGenerator != nullptr){
        delete imageGenerator;
        imageGenerator = nullptr;
    }
}

void Simulator::start()
{
    menu = new Menu();
    menu->setSimulator(this);
    menu->execute();
}

void Simulator::executeDebugger()
{
    // cria o tratador de operacoes I/O
    if(isThereAnIO_Operation())
        io_handler = new IO_Handler();
    
    // cria o gerador de imagem
    if(imageGenerator == nullptr)
        imageGenerator = new GanttChartGenerator(); 

    // TO-DO: invés de chamar o getMaxEntryTime com O(n), fazer o createAxis depois de ordenar as tarefas por ordem de chegada
    imageGenerator->createAxis(tasks.size(), getIdTasks(), sumDurationTasks(), sumIO_DurationTasks(), getMaxEntryTime());

    // ordena as tarefas por ordem de entrada
    std::sort(tasks.begin(), tasks.end(), 
        [](const TCB* t1, const TCB* t2){
            return t1->getEntryTime() < t2->getEntryTime();
        }
    );

    TCB* currentTask = nullptr;
    unsigned int timeLastInterrupt = 0; // in ticks

    // cria o relogio global simulado
    const unsigned int deltaTime = 1.0;
    unsigned int globalClock = 0;

    // quantum
    unsigned int currentTaskQuantum = 0;

    // enquanto houver tarefas nao finalizadas no simulador (na "memoria")
    while(IsThereAnUnfinishedTask()){

        executeDefault(&currentTask, &globalClock, &deltaTime, &currentTaskQuantum, &timeLastInterrupt);

        // Mostra informacoes
        if(currentTask != nullptr)
            chosenMode(currentTask->getId(), globalClock, currentTaskQuantum);
        else
            chosenMode(std::numeric_limits<unsigned int>::max(), globalClock, currentTaskQuantum);

        // atualiza relogio
        globalClock += deltaTime;
    }

    generateImage();
}

void Simulator::executeNoDebugger()
{
    // cria o tratador de operacoes I/O
    if(isThereAnIO_Operation())
        io_handler = new IO_Handler();

    // cria o gerador de imagem
    if(imageGenerator == nullptr)
        imageGenerator = new GanttChartGenerator(); 

    // TO-DO: invés de chamar o getMaxEntryTime com O(n), fazer o createAxis depois de ordenar as tarefas por ordem de chegada
    imageGenerator->createAxis(tasks.size(), getIdTasks(), sumDurationTasks(), sumIO_DurationTasks(), getMaxEntryTime());

    // ordena as tarefas por ordem de entrada
    std::sort(tasks.begin(), tasks.end(), 
        [](const TCB* t1, const TCB* t2){
            return t1->getEntryTime() < t2->getEntryTime();
        }
    );

    TCB* currentTask = nullptr;
    unsigned int timeLastInterrupt = 0; // in ticks

    // cria o relogio global simulado
    const unsigned int deltaTime = 1.0;
    unsigned int globalClock = 0;

    // cria o relogio associado ao quantum
    unsigned int currentTaskQuantum = 0;

    // enquanto houver tarefas nao finalizadas no simulador (na "memoria")
    while(IsThereAnUnfinishedTask()){
        
        executeDefault(&currentTask, &globalClock, &deltaTime, &currentTaskQuantum, &timeLastInterrupt);

        // atualiza relogio
        globalClock += deltaTime;
    }

    generateImage();
}

// método auxiliar que ocorre igualmente em executeDebugger e executeNoDebugger
void Simulator::executeDefault(TCB **currentTask, unsigned int *globalClock, const unsigned int *deltaTime, unsigned int *currentTaskQuantum, unsigned int *timeLastInterrupt)
{
    if(*globalClock > 0){
        // desenha as tarefas prontas
        imageGenerator->addRectsTasks(scheduler->getReadyTasksId(), scheduler->getReadyTasksColor(), *globalClock, *globalClock - *deltaTime);

        // desenha as tarefas suspensas
        imageGenerator->addRectsTasks(io_handler->getSuspendedTasksId(), io_handler->getSuspendedTasksColor(), *globalClock, *globalClock - *deltaTime);
    }

    // determina se o getNextTask do escalonador deve ser chamado
    bool interrupt_flag = false;

    // atualiza as tarefas suspensas
    io_handler->updateSuspendedTasks(*deltaTime);

    // verifica se o tempo atual corresponde a alguma tarefa que pode entrar no escalonador
    std::vector<unsigned int> indexTasks;
    if(canAnyTasksEnter(*globalClock, indexTasks)){

        // adiciona a(s) tarefa(s) na fila de prontas do escalonador
        size_t tam = indexTasks.size();
        for(size_t i = 0; i < tam; i++)
            scheduler->addTask(tasks[indexTasks[i]], getAlpha());
        
        interrupt_flag = true;
    }

    // reliza as ações devidas a tarefa atual
    if(*currentTask != nullptr){
        // atualiza o quatum da tarefa
        *currentTaskQuantum += *deltaTime;

        // atualiza o tempo restante da tarefa
        (*currentTask)->setRemainingTime((*currentTask)->getRemainingTime() - *deltaTime);

        // gera interrupcao somente se acontecer os casos abaixo
        interrupt_flag = 
            (*currentTask)->getRemainingTime() <= 0 || 
            (getQuantum() > 0 && *currentTaskQuantum >= getQuantum()) || 
            io_handler->canAnyIO_OperationBegin(*currentTask);
    }

    // Se houve interrupcao
    if(interrupt_flag){   
        // 1. realizo acoes gerais para tratar a interrupcao

        // desenha na imagem o que aconteceu no processador ate agora
        // em base na tarefa atual
        if(*currentTask != nullptr){
            if((*currentTask)->getColor() != 0) // verifica o formato da cor (antigo ou hexadecimal)
                imageGenerator->addRectTask((*currentTask)->getId(), (*currentTask)->getColor(), *globalClock, *timeLastInterrupt);
            else
                imageGenerator->addRectTask((*currentTask)->getId(), (*currentTask)->getStrColor(), *globalClock, *timeLastInterrupt);
        }

        // atualiza o tempo da ultima interrupcao
        *timeLastInterrupt = *globalClock;

        // 2. realizo acoes especiais para tratar a interrupcao

        // se o tempo restante da tarefa "executada" acabou
        if(*currentTask != nullptr && (*currentTask)->getRemainingTime() <= 0){
            
            // calcula o tempo de fim da tarefa
            (*currentTask)->setEndTime(*globalClock);

            // marca como finalizado a tarefa
            (*currentTask)->setState(TCB::State::Finished);

            // remove a tarefa na fila de prontas do escalonador
            scheduler->removeTask((*currentTask)->getId());

            // "executa" outra tarefa no processador
            (*currentTask) = scheduler->getNextTask();

            // atualiza o quantum da tarefa
            *currentTaskQuantum = 0;
        }
        // se alguma operacao I/O da tarefa pode comecar
        else if(*currentTask != nullptr && io_handler->canAnyIO_OperationBegin(*currentTask)){
            // atualiza o quantum da tarefa
            *currentTaskQuantum = 0;

            // marca a tarefa como suspensa
            (*currentTask)->setState(TCB::State::Suspended);

            // removo da fila de prontas no escalonador
            scheduler->removeTask((*currentTask)->getId());

            // adiciono a tarefa na fila de suspendidos
            io_handler->addSuspendedTask(*currentTask);

            // "executa" outra tarefa no processador
            (*currentTask) = scheduler->getNextTask();
        }
        // se o tempo de quantum da tarefa acabou
        else if(getQuantum() > 0 && *currentTaskQuantum >= getQuantum()){
            // atualiza o quantum da tarefa
            *currentTaskQuantum = 0;

            scheduler->taskQuantumEnded();

            // "executa" outra tarefa no processador
            (*currentTask)->setState(TCB::State::Ready);
            (*currentTask) = scheduler->getNextTask();
        }
        // se alguma tarefa entrou no escalonador, e existe uma tarefa "executada"
        else if(*currentTask != nullptr && indexTasks.size()){
            unsigned int previousTaskId = (*currentTask)->getId();

            // "executa" outra tarefa no processador
            (*currentTask)->setState(TCB::State::Ready);
            (*currentTask) = scheduler->getNextTask();

            // verifica se houve uma troca de tarefas
            if(previousTaskId != (*currentTask)->getId())
                *currentTaskQuantum = 0;
        }
        // se alguma tarefa entrou no escalonador, e nao existe uma tarefa "executada"
        else if(indexTasks.size()){
            // "executa" outra tarefa no processador
            (*currentTask) = scheduler->getNextTask();

            *currentTaskQuantum = 0;
        }
    }

    if(*currentTask != nullptr)
        (*currentTask)->setState(TCB::State::Running);
}

// função auxiliar para trim (remove espaços no início e no fim)
void Simulator::trim(std::string &s) {
    // remove espaços à esquerda
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch){ return !std::isspace(ch); }));
    // remove espaços à direita
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());
}

// função para remover todos os '\r' (útil para arquivos Windows)
void Simulator::remove_cr(std::string &s) {
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
}

const bool Simulator::existId(unsigned int id)
{
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        if(tasks[i]->getId() == id)
            return true;
    
    return false;
}

unsigned int Simulator::modifyId(unsigned int id)
{
    static std::mt19937 gen(std::random_device{}());

    unsigned int newId = id;
    unsigned int factor = 2;
    const int maxFactor = 1000000; // seguranca
    unsigned int numAttempts = 0;
    
    // metodo escolhido: dou preferencia aos numeros menores
    while(existId(newId)){
        std::uniform_int_distribution<unsigned int> dist(0, factor - 1);
        newId = dist(gen);

        numAttempts++;

        if(numAttempts >= factor / 2 && factor < maxFactor)
            factor *= 2;
    }

    return newId;
}

// Usado no depurador
// Cria uma tarefa, com as informacoes do idTask passado, no heap
void Simulator::createTask(std::vector<TCB*>& pTasks, unsigned int idTask)
{
    // Encontra a tarefa a ser copiada
    std::vector<TCB*>::iterator it;
    for(it = tasks.begin(); (*it)->getId() != idTask; it++);

    if (it == tasks.end()) return; // seguranca. Nao faz mal a ninguem, ne?

    TCB* newTask = new TCB();
    newTask->copyTCB(*(*it));

    // Encontra a tarefa a ser copiada
    std::vector<TCB*>::iterator it2;
    for(it2 = pTasks.begin(); (*it2)->getId() != idTask; it2++);

    if (it2 == pTasks.end()) return; // seguranca

    (*it2) = newTask; 
}

void Simulator::chosenMode(const unsigned int& currentIdTask, const unsigned int& globalClock, const unsigned int& currentTaskQuantum)
{
    menu->clearTerminal();

    showMinimumInfo(currentIdTask, globalClock, currentTaskQuantum);

    std::cout << 
        "Enter the number of the desired option:\n\n" <<
        "1. See all information for each task;\n" <<
        "2. View information about tasks in the ready queue, and information about the executed task;\n" <<
        "3. View information about tasks in the suspended set, and information about the executed task;\n"
        "4. Proceed to the next instant of time;\n" <<
    std::endl;

    int option = menu->checkEntryNumber(1, 4);

    if(option == 1){
        showAllTasks(currentIdTask, globalClock, currentTaskQuantum);
    }
    else if(option == 2){
        showReadyTasks(currentIdTask, globalClock, currentTaskQuantum);
    }
    else if(option == 3){
        showSuspendedTasks(currentIdTask, globalClock, currentTaskQuantum);
    }

    menu->clearTerminal();
}

void Simulator::showMinimumInfo(const unsigned int& currentIdTask, const unsigned int& globalClock, const unsigned int& currentTaskQuantum)
{
    std::cout 
        << "Algorithm scheduler: " << getAlgorithmScheduler() << "\n"
        << "Quantum value: " << getQuantum() << "\n"
        << "Current time instant: " << globalClock << "\n"
        << "Current Task quantum: " << currentTaskQuantum << "\n";

    if(currentIdTask == std::numeric_limits<unsigned int>::max())
        std::cout << "Executed Task ID: none" << "\n" << std::endl;
    else
        std::cout << "Executed Task ID: " << currentIdTask << "\n" << std::endl;
}

void Simulator::showAllTasks(const unsigned int& currentIdTask, const unsigned int& globalClock, const unsigned int& currentTaskQuantum)
{
    menu->clearTerminal();

    showMinimumInfo(currentIdTask, globalClock, currentTaskQuantum);

    // Mostra as informacoes das tarefas para o usuario
    menu->showTasks(tasks);

    std::cout << "Press enter to return to the options menu" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cout
    std::cin.get();

    chosenMode(currentIdTask, globalClock, currentTaskQuantum);

    menu->clearTerminal();
}

void Simulator::showReadyTasks(const unsigned int &currentIdTask, const unsigned int &globalClock, const unsigned int& currentTaskQuantum)
{
    menu->clearTerminal();

    showMinimumInfo(currentIdTask, globalClock, currentTaskQuantum);

    // Mostra as informacoes das tarefas para o usuario
    std::cout << "Information about tasks in the ready queue, and the executed task:\n" << std::endl;

    menu->showTasks(scheduler->getTasks());

    std::cout << "Press enter to return to the options menu" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cout
    std::cin.get();

    chosenMode(currentIdTask, globalClock, currentTaskQuantum);

    menu->clearTerminal();
}

void Simulator::showSuspendedTasks(const unsigned int &currentIdTask, const unsigned int &globalClock, const unsigned int &currentTaskQuantum)
{
    menu->clearTerminal();

    showMinimumInfo(currentIdTask, globalClock, currentTaskQuantum);

    // Mostra as informacoes das tarefas para o usuario
    std::cout << "Information about tasks in the suspended set:\n" << std::endl;

    menu->showTasks(io_handler->getSuspendedTasks());

    std::cout << "Press enter to return to the options menu" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cout
    std::cin.get();

    chosenMode(currentIdTask, globalClock, currentTaskQuantum);

    menu->clearTerminal();
}

std::vector<TCB*> Simulator::loadArquive() {
    std::ifstream file("configuration.txt");
    
    if (!file.is_open()) {
        std::cerr << "Error opening configuration file" << std::endl;
        return std::vector<TCB*>();
    }

    std::string line;

    // 1) Ler primeira linha inteira: algoritmo;quantum;alpha
    if (!std::getline(file, line)) {
        std::cerr << "Configuration file is empty" << std::endl;
        return std::vector<TCB*>();
    }
    
    remove_cr(line);
    trim(line);
    if (line.empty()) {
        std::cerr << "First line is empty" << std::endl;
        return std::vector<TCB*>();
    }

    // parse da primeira linha
    std::istringstream ss(line);

    std::string alg;
    std::string quantumStr;
    std::string alphaStr;

    // --- algoritmo ---
    if (!std::getline(ss, alg, ';')) {
        std::cerr << "Malformed first line: missing algorithm" << std::endl;
        return std::vector<TCB*>();
    }
    remove_cr(alg);
    trim(alg);
    setAlgorithmScheduler(alg);

    // --- quantum ---
    if (!std::getline(ss, quantumStr, ';')) {
        std::cerr << "Malformed first line: missing quantum" << std::endl;
        return std::vector<TCB*>();
    }
    remove_cr(quantumStr);
    trim(quantumStr);

    // valida quantum
    try {
        int q = std::stoi(quantumStr);
        extraInfo.setQuantum(q);
    } catch (...) {
        std::cerr << "Invalid quantum value: '" << quantumStr << "'" << std::endl;
        return std::vector<TCB*>();
    }

    // --- alpha ---
    if (std::getline(ss, alphaStr, ';')) {
        remove_cr(alphaStr);
        trim(alphaStr);

        // valida alpha
        try {
            int a = std::stoi(alphaStr);
            extraInfo.setAlpha(a);
        } catch (...) {
            std::cerr << "Invalid alpha value: '" << alphaStr << "'" << std::endl;
            return std::vector<TCB*>();
        }
    }

    // 2) Ler o resto das linhas — cada linha é uma tarefa no formato:
    // id;color;entry;duration;priority
    while (std::getline(file, line)) {
        remove_cr(line);
        trim(line);
        if (line.empty()) continue; // pula linhas em branco

        std::istringstream ls(line);
        std::string field;
        TCB* task = new TCB();

        // funcoes lambda auxiliares
        auto safeGetInt = [&](int &out) -> bool {
            if (!std::getline(ls, field, ';')) return false;
            remove_cr(field);
            trim(field);
            if (field.empty()) return false;
            try {
                out = std::stoi(field);
                return true;
            } catch (...) {
                return false;
            }
        };

        auto safeGetString = [&](std::string &out) -> bool {
            if(!std::getline(ls, field, ';')) return false;
            remove_cr(field);
            trim(field);
            if(field.empty()) return false;
            out.clear(); // garantia
            out = field;
            return true;
        };

        int tmp;
        std::string strTmp;
        
        // ID
        // Diferente dos demais, pois o id pode comecar com t ou nao
        if(safeGetString(strTmp)){
            if(strTmp[0] == 't')
                strTmp.erase(strTmp.begin());
            
            try{
                tmp = std::stoi(strTmp);
            }
            catch (...){
                std::cerr << "Bad or missing ID in line: " << line << std::endl; 
                continue;
            }
        }
        else{
            std::cerr << "Bad or missing ID in line: " << line << std::endl; 
            continue;
        }

        // verifica se o ID ja existe
        if(existId(tmp)){
            // muda o valor dele
            int prev = tmp;
            tmp = modifyId(tmp);

            std::cout << "The task ID " << prev << " was modified for " << tmp << ", because " << prev << " already exists.\n" << std::endl;
        }

        task->setId(tmp);
        
        // Color
        if(safeGetString(strTmp)){
            // Verifica se esta no formato hexadecimal
            if(strTmp.size() == 6 && std::all_of(strTmp.begin(), strTmp.end(), [](unsigned char c){return std::isxdigit(c);})){
                task->setColor(strTmp);
            }
            // Verifica se esta no formato antigo
            else if(strTmp[0] == '1' || strTmp[0] == '2' || strTmp[0] == '3' || strTmp[0] == '4' || strTmp[0] == '5' || strTmp[0] == '6'){
                task->setColor(strTmp[0] - '0');
            }
            else{
                std::cerr << "Bad or missing Color in line: " << line << std::endl; 
                continue; 
            }
        }

        // Entry time
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing EntryTime in line: " << line << std::endl; 
            continue; 
        }
        task->setEntryTime(tmp);
        
        // Duration
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing Duration in line: " << line << std::endl; 
            continue; 
        }
        task->setDuration(tmp);
        
        // Priority
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing Priority in line: " << line << std::endl; 
            continue; 
        }
        task->setStaticPriority(tmp);

        // Operações I/O
        while (safeGetString(strTmp)) {

            // verifica formato IO:xx-yy
            if (strTmp.rfind("IO:", 0) != 0) {
                std::cerr << "Bad or missing IO operation in line: " << line << std::endl;
                continue;
            }

            // remove o prefixo "IO:"
            std::string data = strTmp.substr(3); // agora deve ser "xx-yy"

            // encontra o separador '-'
            size_t sep = data.find('-');
            if (sep == std::string::npos) {
                std::cerr << "Malformed IO operation in line: " << line << std::endl;
                continue;
            }

            std::pair<unsigned int, unsigned int> IO;

            // captura tempo inicial
            try {
                IO.first = std::stoi(data.substr(0, sep));
            } catch (...) {
                std::cerr << "Bad IO initial time in line: " << line << std::endl;
                continue;
            }

            // captura duração
            try {
                IO.second = std::stoi(data.substr(sep + 1));
            } catch (...) {
                std::cerr << "Bad IO duration time in line: " << line << std::endl;
                continue;
            }

            // salva os dados
            if (!task->addIO_operation(IO)) {
                std::cerr << "The IO operation in line: " << line << " cannot be inserted." << std::endl;
                continue;
            }
        }

        tasks.push_back(task);
    }

    file.close();
    return tasks;
}

void Simulator::generateImage()
{
    imageGenerator->generateImage();
}

void Simulator::addTask(TCB task)
{
    TCB* t = new TCB();
    t->copyTCB(task);

    tasks.push_back(t);
}

// task tem que existir em tasks
void Simulator::updateTask(const TCB *task)
{
    // Procura o Id da task em tasks
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != task->getId(); i++);

    // atualiza
    tasks[i]->copyTCB(*task);
}

/*
Requisitos:
o idTask tem que existir em tasks
o newId nao pode ser igual a outro id de alguma tarefa
*/
void Simulator::updateTaskId(const unsigned int &idTask, const unsigned int &newId)
{
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != idTask; i++);

    tasks[i]->setId(newId);
}

/*
Requisitos:
o idTask tem que existir em tasks
*/
void Simulator::updateTaskColor(const unsigned int &idTask, const unsigned int &newColor)
{
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != idTask; i++);

    tasks[i]->setColor(newColor);
}

/*
Requisitos:
o idTask tem que existir em tasks
*/
void Simulator::updateTaskColor(const unsigned int &idTask, const std::string &newStrColor)
{
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != idTask; i++);

    tasks[i]->setColor(newStrColor);
}

/*
Requisitos:
o idTask tem que existir em tasks
*/
void Simulator::updateTaskEntryTime(const unsigned int &idTask, const unsigned int &newEntryTime)
{
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != idTask; i++);

    tasks[i]->setEntryTime(newEntryTime);
}

/*
Requisitos:
o idTask tem que existir em tasks
*/
void Simulator::updateTaskDuration(const unsigned int &idTask, const unsigned int &newDuration)
{
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != idTask; i++);

    tasks[i]->setDuration(newDuration);
}

/*
Requisitos:
o idTask tem que existir em tasks
*/
void Simulator::updateTaskStaticPriority(const unsigned int &idTask, const unsigned int &newPriority)
{
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != idTask; i++);

    tasks[i]->setStaticPriority(newPriority);
}

/*
Requisitos:
o taskId tem que existir em tasks
*/
const bool Simulator::updateTaskIO_InitialTime(const unsigned int& taskId, const unsigned int& oldInitialTime, const unsigned int& newInitialTime)
{
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != taskId; i++);

    // pega a operacao I/O especifica
    IO_Operation op = tasks[i]->getIO_operation(oldInitialTime);
    tasks[i]->removeIO_operation(oldInitialTime);
    
    return tasks[i]->addIO_operation(std::pair<unsigned int, unsigned int>{newInitialTime, op.getDuration()});
}

/*
Requisitos:
o taskId tem que existir em tasks
*/
const bool Simulator::updateTaskIO_Duration(const unsigned int& taskId, const unsigned int& initialTime, const unsigned int& newDuration)
{
    size_t tam = tasks.size();
    size_t i;
    for(i = 0; tasks[i]->getId() != taskId; i++);

    tasks[i]->removeIO_operation(initialTime);

    return tasks[i]->addIO_operation(std::pair<unsigned int, unsigned int>{initialTime, newDuration});
}

void Simulator::setAlgorithmScheduler(int i)
{
    if(scheduler == nullptr)
        scheduler = new Scheduler();
    
    if(i == 1){
        extraInfo.setAlgorithmScheduler("FIFO");

        scheduler->setAlgorithm(Scheduler::Algorithm::FIFO);
    }
    else if(i == 2){
        extraInfo.setAlgorithmScheduler("SRTF");

        scheduler->setAlgorithm(Scheduler::Algorithm::SRTF);
    }
    else if(i == 3){
        extraInfo.setAlgorithmScheduler("PRIOp");

        scheduler->setAlgorithm(Scheduler::Algorithm::PRIOp);
    }
    else if(i == 4){
        extraInfo.setAlgorithmScheduler("PRIOPEnv");

        scheduler->setAlgorithm(Scheduler::Algorithm::PRIOPEnv);
    }
}

void Simulator::setAlgorithmScheduler(std::string algorithm)
{
    extraInfo.setAlgorithmScheduler(algorithm);

    if(scheduler == nullptr)
        scheduler = new Scheduler();

    if(algorithm == "FIFO" || algorithm == "FCFS" || algorithm == "RR")
        scheduler->setAlgorithm(Scheduler::Algorithm::FIFO);
    else if(algorithm == "SRTF")
        scheduler->setAlgorithm(Scheduler::Algorithm::SRTF);
    else if(algorithm == "PRIOp")
        scheduler->setAlgorithm(Scheduler::Algorithm::PRIOp);
    else if(algorithm == "PRIOPEnv")
        scheduler->setAlgorithm(Scheduler::Algorithm::PRIOPEnv);
}

void Simulator::setQuantum(unsigned int q)
{
    extraInfo.setQuantum(q);
}

void Simulator::setAlpha(unsigned int a)
{
    extraInfo.setAlpha(a);
}

std::vector<TCB*> Simulator::getTasks() const
{
    return tasks;
}

std::string Simulator::getAlgorithmScheduler() const
{
    return extraInfo.getAlgorithmScheduler();
}

unsigned int Simulator::getQuantum() const
{
    return extraInfo.getQuantum();
}

unsigned int Simulator::getAlpha() const
{
    return extraInfo.getAlpha();
}

double Simulator::calcTicksPerSecond()
{
    // Soma total dos ticks de todas as tarefas
    unsigned int totalTicks = 0;
    for(const auto* task : tasks)
        totalTicks += task->getDuration();

    // Define a duração da simulação (em segundos) na imagem final
    double durationSimulator = 10.0;

    // ticks por segundo
    return static_cast<double>(totalTicks) / durationSimulator;
}

unsigned int Simulator::sumDurationTasks()
{
    unsigned int sum = 0;

    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        sum += tasks[i]->getDuration();

    return sum;
}

unsigned int Simulator::sumIO_DurationTasks()
{
    unsigned int sum = 0;

    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++){
        std::vector<IO_Operation> v = tasks[i]->getIO_operations();
        size_t tam2 = v.size();
        for(size_t j = 0; j < tam2; j++){
            sum += v[j].getDuration();
        }
    }

    return sum;
}

std::vector<unsigned int> Simulator::getIdTasks()
{
    std::vector<unsigned int> idTasks(tasks.size());

    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        idTasks[i] = tasks[i]->getId();

    return idTasks;
}

unsigned int Simulator::getMaxEntryTime()
{
    unsigned int max = 0;

    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        if(max < tasks[i]->getEntryTime())
            max = tasks[i]->getEntryTime();

    return max;
}

const bool Simulator::isThereAnIO_Operation()
{
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        if(tasks[i]->getIO_operations().size())
            return true;

    return false;
}

// timeNow em ticks
const bool Simulator::canAnyTasksEnter(double timeNow, std::vector<unsigned int>& indexTasks)
{
    bool found = false;

    for (unsigned int i = 0; i < tasks.size(); i++)
    {
        // só entra se nunca tiver sido adicionada ao escalonador
        if (tasks[i]->getState() == TCB::State::New &&
            timeNow >= tasks[i]->getEntryTime())
        {
            indexTasks.push_back(i);
            found = true;
        }
    }

    return found;
}

const bool Simulator::IsThereAnUnfinishedTask()
{
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        if(tasks[i]->getState() != TCB::State::Finished)
            return true;

    return false;
}