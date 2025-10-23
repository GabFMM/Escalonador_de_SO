#include "Simulator.h"
#include "Menu.h"

Simulator::Simulator() :
menu(nullptr),
scheduler(nullptr),
imageGenerator(nullptr)
{

}

Simulator::~Simulator()
{
    tasks.clear();

    if(menu != nullptr){
        delete menu;
        menu = nullptr;
    }

    if(scheduler != nullptr){
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
    // Ponteiro para usar no depurador
    std::vector<TCB*> ptasks;
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        ptasks[i] = &(tasks[i]);

    // cria o gerador de imagem
    if(imageGenerator == nullptr)
        imageGenerator = new GanttChartGenerator(); 

    // TO-DO: invés de chamar o getMaxEntryTime com O(n), fazer o createAxis depois de ordenar as tarefas por ordem de chegada
    imageGenerator->createAxis(tasks.size(), getIdTasks(), sumDurationTasks(), getMaxEntryTime());

    // ordena as tarefas por ordem de entrada
    std::sort(tasks.begin(), tasks.end(), 
        [](const TCB& t1, const TCB& t2){
            return t1.getEntryTime() < t2.getEntryTime();
        }
    );

    TCB* currentTask = nullptr;
    unsigned int timeLastInterrupt = 0; // in ticks

    // calcula o tick/segundo
    double tps = calcTicksPerSecond();

    // cria o relogio global simulado
    const double deltaTime = 1.0;
    unsigned int globalClock = 0;

    // enquanto houver tarefas no simulador (na "memoria")
    while(tasks.size()){ 
        // verifica se o tempo atual corresponde a alguma tarefa que pode entrar no escalonador
        unsigned int indexTask = 0;
        if(canAnyTaskEnter(globalClock, &indexTask, scheduler->getIdTasks())){

            // ignora a primeira interrupcao
            if(currentTask != nullptr){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask->getId(), currentTask->getColor(), globalClock, timeLastInterrupt);

                // Atualiza o tempo restante da tarefa executada
                currentTask->setRemainingTime(currentTask->getRemainingTime() - deltaTime);

                if(currentTask->getRemainingTime() <= 0){
                    // remove a tarefa na fila de prontas do simulator
                    int id = currentTask->getId();
                    removeTask(id);


                    // remove a tarefa na fila de prontas do escalonador
                    scheduler->removeTask(id);

                    // Recalcula o valor do index para nao acessar memoria invalida
                    canAnyTaskEnter(globalClock, &indexTask, scheduler->getIdTasks());
                }
            }

            timeLastInterrupt = globalClock;

            // adiciona a tarefa na fila de prontas do escalonador
            tasks[indexTask].setLastUsedTime(globalClock);
            scheduler->addTask(tasks[indexTask]);

            // "executa" a tarefa no processador
            currentTask = scheduler->getNextTask();
        }
        // Verifica se o tempo restante da tarefa executada acabou 
        else if(currentTask != nullptr){
            currentTask->setRemainingTime(currentTask->getRemainingTime() - deltaTime);

            if(currentTask->getRemainingTime() <= 0){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask->getId(), currentTask->getColor(), globalClock, timeLastInterrupt);

                timeLastInterrupt = globalClock;

                // remove a tarefa na fila de prontas do simulator
                int id = currentTask->getId();
                removeTask(id);

                // remove a tarefa na fila de prontas do escalonador
                scheduler->removeTask(id);

                // "executa" outra tarefa no processador
                currentTask = scheduler->getNextTask();

                if(currentTask != nullptr)
                    currentTask->setLastUsedTime(globalClock);
            }
        }
        // Mostra o tempo atual para o usuario
        std::cout << "Time (in ticks) now: " << globalClock << "\n" << std::endl;

        // Mostra as informacoes das tarefas para o usuario
        std::cout << "Tasks information:\n" << std::endl;
        size_t tam = tasksCopy.size();
        for(size_t i = 0; i < tam; i++){
            std::cout << 
            "ID: " << tasksCopy[i].getId() << "\n" <<
            "Color: " << tasksCopy[i].getColor() << "\n" <<
            "Entry time: " << tasksCopy[i].getEntryTime() << "\n" <<
            "Duration: " << tasksCopy[i].getDuration() << "\n" <<
            "Priority: " << tasksCopy[i].getPriority() << "\n" <<
            std::endl;
        }

        // atualiza relogio
        globalClock += deltaTime;
    }

    deleteTasks(ptasks);
    generateImage();
}

void Simulator::executeNoDebugger()
{
    // cria o gerador de imagem
    if(imageGenerator == nullptr)
        imageGenerator = new GanttChartGenerator(); 

    // TO-DO: invés de chamar o getMaxEntryTime com O(n), fazer o createAxis depois de ordenar as tarefas por ordem de chegada
    imageGenerator->createAxis(tasks.size(), getIdTasks(), sumDurationTasks(), getMaxEntryTime());

    // ordena as tarefas por ordem de entrada
    std::sort(tasks.begin(), tasks.end(), 
        [](const TCB& t1, const TCB& t2){
            return t1.getEntryTime() < t2.getEntryTime();
        }
    );

    TCB* currentTask = nullptr;
    unsigned int timeLastInterrupt = 0; // in ticks

    // calcula o tick/segundo
    double tps = calcTicksPerSecond();

    // cria o relogio global simulado
    const double deltaTime = 1.0;
    unsigned int globalClock = 0;

    // enquanto houver tarefas no simulador (na "memoria")
    while(tasks.size()){ 
        // verifica se o tempo atual corresponde a alguma tarefa que pode entrar no escalonador
        unsigned int indexTask = 0;
        if(canAnyTaskEnter(globalClock, &indexTask, scheduler->getIdTasks())){

            // ignora a primeira interrupcao
            if(currentTask != nullptr){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask->getId(), currentTask->getColor(), globalClock, timeLastInterrupt);

                // Atualiza o tempo restante da tarefa executada
                currentTask->setRemainingTime(currentTask->getRemainingTime() - deltaTime);

                if(currentTask->getRemainingTime() <= 0){
                    // remove a tarefa na fila de prontas do simulator
                    int id = currentTask->getId();
                    removeTask(id);

                    // remove a tarefa na fila de prontas do escalonador
                    scheduler->removeTask(id);

                    // Recalcula o valor do index para nao acessar memoria invalida
                    canAnyTaskEnter(globalClock, &indexTask, scheduler->getIdTasks());
                }
            }

            timeLastInterrupt = globalClock;

            // adiciona a tarefa na fila de prontas do escalonador
            tasks[indexTask].setLastUsedTime(globalClock);
            scheduler->addTask(tasks[indexTask]);

            // "executa" a tarefa no processador
            currentTask = scheduler->getNextTask();
        }
        // Verifica se o tempo restante da tarefa executada acabou 
        else if(currentTask != nullptr){
            currentTask->setRemainingTime(currentTask->getRemainingTime() - deltaTime);

            if(currentTask->getRemainingTime() <= 0){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask->getId(), currentTask->getColor(), globalClock, timeLastInterrupt);

                timeLastInterrupt = globalClock;

                // remove a tarefa na fila de prontas do simulator
                int id = currentTask->getId();
                removeTask(id);

                // remove a tarefa na fila de prontas do escalonador
                scheduler->removeTask(id);

                // "executa" outra tarefa no processador
                currentTask = scheduler->getNextTask();

                if(currentTask != nullptr)
                    currentTask->setLastUsedTime(globalClock);
            }
        }
        // atualiza relogio
        globalClock += deltaTime;
    }

    generateImage();
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

// Usado no depurador
// Cria uma tarefa, com as informacoes do idTask passado, no heap
void Simulator::createTask(std::vector<TCB*>& pTasks, int idTask)
{
    // Encontra a tarefa a ser copiada
    std::vector<TCB>::iterator it;
    for(it = tasks.begin(); it->getId() != idTask; it++);

    TCB* newTask = new TCB();
    newTask->copyTCB((*it));

    // Encontra a tarefa a ser copiada
    std::vector<TCB*>::iterator it2;
    for(it2 = pTasks.begin(); (*it2)->getId() != idTask; it2++);

    (*it2) = newTask; 
}

void Simulator::deleteTasks(std::vector<TCB*>& pTasks)
{
    size_t tam = pTasks.size();
    for(size_t i = 0; i < tam; i++){
        if(pTasks[i] != nullptr){
            delete pTasks[i];
            pTasks[i] = nullptr;
        }
    }
}

std::vector<TCB> Simulator::loadArquive() {
    std::ifstream file("../assets/configuration.txt");
    
    if (!file.is_open()) {
        std::cerr << "Error opening configuration file" << std::endl;
        return std::vector<TCB>();
    }

    std::string line;

    // 1) Ler primeira linha inteira: algoritmo;quantum
    if (!std::getline(file, line)) {
        std::cerr << "Configuration file is empty" << std::endl;
        return std::vector<TCB>();
    }
    
    remove_cr(line);
    trim(line);
    if (line.empty()) {
        std::cerr << "First line is empty" << std::endl;
        return std::vector<TCB>();
    }

    // parse da primeira linha
    std::istringstream ss(line);
    std::string alg;
    std::string quantumStr;

    if (!std::getline(ss, alg, ';')) {
        std::cerr << "Malformed first line: missing algorithm" << std::endl;
        return std::vector<TCB>();
    }
    remove_cr(alg);
    trim(alg);
    setAlgorithmScheduler(alg);

    if (!std::getline(ss, quantumStr, ';')) {
        // tentar sem delimitador adicional (se não houver ';' depois)
        // neste caso quantumStr já foi lido como resto da linha por getline com '\n'
        // mas getline com ';' retorna false se não há ';'
        ss.clear();
        ss.seekg(0);
        // alternativa: ler o resto da stream
        ss.str(line); // reatribui
        // consumir até o ';' novamente
        size_t pos = line.find(';');
        if (pos != std::string::npos && pos + 1 < line.size()) {
            quantumStr = line.substr(pos + 1);
        } else {
            std::cerr << "Malformed first line: missing quantum" << std::endl;
            return std::vector<TCB>();
        }
    }
    remove_cr(quantumStr);
    trim(quantumStr);
    if (quantumStr.empty()) {
        std::cerr << "Quantum empty in configuration" << std::endl;
        return std::vector<TCB>();
    }
    try {
        extraInfo.setQuantum(std::stoi(quantumStr));
    } catch (const std::exception &e) {
        std::cerr << "Invalid quantum value: '" << quantumStr << "'" << std::endl;
        return std::vector<TCB>();
    }

    // 2) Ler o resto das linhas — cada linha é uma tarefa no formato:
    // id;color;entry;duration;priority
    while (std::getline(file, line)) {
        remove_cr(line);
        trim(line);
        if (line.empty()) continue; // pula linhas em branco

        std::istringstream ls(line);
        std::string field;
        TCB task;

        auto safeGetInt = [&](int &out) -> bool {
            if (!std::getline(ls, field, ';')) return false;
            remove_cr(field);
            trim(field);
            if (field.empty()) return false;
            try {
                out = std::stoi(field);
                return true;
            } catch (const std::exception &e) {
                return false;
            }
        };

        int tmp;
        // ID
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing ID in line: " << line << std::endl; 
            continue; 
        }
        task.setId(tmp);
        
        // Color
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing Color in line: " << line << std::endl; 
            continue; 
        }
        task.setColor(tmp);
        
        // Entry time
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing EntryTime in line: " << line << std::endl; 
            continue; 
        }
        task.setEntryTime(tmp);
        
        // Duration
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing Duration in line: " << line << std::endl; 
            continue; 
        }
        task.setDuration(tmp);
        
        // Priority
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing Priority in line: " << line << std::endl; 
            continue; 
        }
        task.setPriority(tmp);

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
    tasks.push_back(task);
}

void Simulator::removeTask(unsigned int idTask)
{
    std::vector<TCB>::iterator it;
    for(it = tasks.begin(); it->getId() != idTask; it++);
    
    tasks.erase(it);
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
}

void Simulator::setAlgorithmScheduler(std::string algorithm)
{
    extraInfo.setAlgorithmScheduler(algorithm);

    if(scheduler == nullptr)
        scheduler = new Scheduler();

    if(algorithm == "FIFO")
        scheduler->setAlgorithm(Scheduler::Algorithm::FIFO);
    else if(algorithm == "SRTF")
        scheduler->setAlgorithm(Scheduler::Algorithm::SRTF);
    else if(algorithm == "PRIOp")
        scheduler->setAlgorithm(Scheduler::Algorithm::PRIOp);
}

void Simulator::setQuantum(unsigned int q)
{
    extraInfo.setQuantum(q);
}

std::vector<TCB> Simulator::getTasks() const
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

double Simulator::calcTicksPerSecond()
{
    // Soma total dos ticks de todas as tarefas
    unsigned int totalTicks = 0;
    for(const auto& task : tasks)
        totalTicks += task.getDuration();

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
        sum += tasks[i].getDuration();

    return sum;
}

std::vector<unsigned int> Simulator::getIdTasks()
{
    std::vector<unsigned int> idTasks(tasks.size());

    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        idTasks[i] = tasks[i].getId();

    return idTasks;
}

unsigned int Simulator::getMaxEntryTime()
{
    unsigned int max = 0;

    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        if(max < tasks[i].getEntryTime())
            max = tasks[i].getEntryTime();

    return max;
}

// timeNow em ticks
const bool Simulator::canAnyTaskEnter(double timeNow, unsigned int* indexTask, const std::vector<int>& exceptionIdTasks)
{
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++){
        if(
            std::find(exceptionIdTasks.begin(), exceptionIdTasks.end(), tasks[i].getId()) == exceptionIdTasks.end() 
            &&
            timeNow >= tasks[i].getEntryTime()
        ){
            (*indexTask) = i;
            return true;
        }
    }
    
    return false;
}
