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
    std::vector<TCB*> ptasks(tasks.size());
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        ptasks[i] = tasks[i];

    // cria o gerador de imagem
    if(imageGenerator == nullptr)
        imageGenerator = new GanttChartGenerator(); 

    // TO-DO: invés de chamar o getMaxEntryTime com O(n), fazer o createAxis depois de ordenar as tarefas por ordem de chegada
    imageGenerator->createAxis(tasks.size(), getIdTasks(), sumDurationTasks(), getMaxEntryTime());

    // ordena as tarefas por ordem de entrada
    std::sort(tasks.begin(), tasks.end(), 
        [](const TCB* t1, const TCB* t2){
            return t1->getEntryTime() < t2->getEntryTime();
        }
    );

    ptasks = tasks; // pois o std::sort muda os enderecos apontados por tasks

    TCB* currentTask = nullptr;
    unsigned int timeLastInterrupt = 0; // in ticks

    // calcula o tick/segundo
    double tps = calcTicksPerSecond();

    // cria o relogio global simulado
    const double deltaTime = 1.0;
    unsigned int globalClock = 0;

    // enquanto houver tarefas no simulador (na "memoria")
    while(tasks.size()){
        // Usado na ultima iteracao do debugger e remocoes da tarefa executada
        int id = -1;

        // verifica se o tempo atual corresponde a alguma tarefa que pode entrar no escalonador
        std::vector<unsigned int> indexTasks;
        if(canAnyTasksEnter(globalClock, indexTasks, scheduler->getIdTasks())){

            // ignora a primeira interrupcao
            if(currentTask != nullptr){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask->getId(), currentTask->getColor(), globalClock, timeLastInterrupt);

                // Atualiza o tempo restante da tarefa executada
                currentTask->setRemainingTime(currentTask->getRemainingTime() - deltaTime);

                if(currentTask->getRemainingTime() <= 0){
                    // calcula o tempo de fim da tarefa
                    currentTask->setEndTime(globalClock);

                    updateTask(currentTask);

                    id = currentTask->getId();

                    // remove a tarefa na fila de prontas do simulator
                    removeTask(id);

                    // remove a tarefa na fila de prontas do escalonador
                    scheduler->removeTask(id);

                    // Recalcula o valor do index para nao acessar memoria invalida
                    canAnyTasksEnter(globalClock, indexTasks, scheduler->getIdTasks());
                }
            }

            timeLastInterrupt = globalClock;

            // adiciona a(s) tarefa(s) na fila de prontas do escalonador
            size_t tam = indexTasks.size();
            for(size_t i = 0; i < tam; i++){
                tasks[indexTasks[i]]->setLastUsedTime(globalClock);
            
                scheduler->addTask(*(tasks[indexTasks[i]]));
            }

            // "executa" a tarefa no processador
            currentTask = scheduler->getNextTask();
        }
        // Verifica se o tempo do quantum acabou
        else if(globalClock % extraInfo.getQuantum() == 0 && currentTask != nullptr){
            // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
            // em base na tarefa atual
            imageGenerator->addRectTask(currentTask->getId(), currentTask->getColor(), globalClock, timeLastInterrupt);

            timeLastInterrupt = globalClock;

            // Atualiza o tempo restante da tarefa executada
            currentTask->setRemainingTime(currentTask->getRemainingTime() - deltaTime);

            if(currentTask->getRemainingTime() <= 0){
                // calcula o tempo de fim da tarefa
                currentTask->setEndTime(globalClock);

                updateTask(currentTask);

                id = currentTask->getId();

                // remove a tarefa na fila de prontas do simulator
                removeTask(id);

                // remove a tarefa na fila de prontas do escalonador
                scheduler->removeTask(id);
            }

            currentTask = scheduler->getNextTask();
        }
        else if(currentTask != nullptr){
            currentTask->setRemainingTime(currentTask->getRemainingTime() - deltaTime);
            updateTask(currentTask);

            if(currentTask->getRemainingTime() <= 0){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask->getId(), currentTask->getColor(), globalClock, timeLastInterrupt);

                timeLastInterrupt = globalClock;

                // calcula o tempo de fim da tarefa
                currentTask->setEndTime(globalClock);

                // atualiza o currentTask correspondente em tasks
                updateTask(currentTask);

                id = currentTask->getId();

                // remove a tarefa na fila de prontas do simulator
                removeTask(id);

                // remove a tarefa na fila de prontas do escalonador
                scheduler->removeTask(id);

                // "executa" outra tarefa no processador
                currentTask = scheduler->getNextTask();
            }
        }

        // Mostra informacoes
        if(currentTask != nullptr){
            chosenMode(ptasks, currentTask->getId(), globalClock);
        }
        else{
            chosenMode(ptasks, id, globalClock); // ocorre na ultima iteracao do debugger
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
        [](const TCB* t1, const TCB* t2){
            return t1->getEntryTime() < t2->getEntryTime();
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
        std::vector<unsigned int> indexTasks;
        if(canAnyTasksEnter(globalClock, indexTasks, scheduler->getIdTasks())){

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
                    canAnyTasksEnter(globalClock, indexTasks, scheduler->getIdTasks());
                }
            }

            timeLastInterrupt = globalClock;

            // adiciona a(s) tarefa(s) na fila de prontas do escalonador
            size_t tam = indexTasks.size();
            for(size_t i = 0; i < tam; i++){
                tasks[indexTasks[i]]->setLastUsedTime(globalClock);
            
                scheduler->addTask(*(tasks[indexTasks[i]]));
            }

            // "executa" a tarefa no processador
            currentTask = scheduler->getNextTask();
        }
        // Verifica se o tempo do quantum acabou
        else if(globalClock % extraInfo.getQuantum() == 0 && currentTask != nullptr){
            // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
            // em base na tarefa atual
            imageGenerator->addRectTask(currentTask->getId(), currentTask->getColor(), globalClock, timeLastInterrupt);

            timeLastInterrupt = globalClock;

            // Atualiza o tempo restante da tarefa executada
            currentTask->setRemainingTime(currentTask->getRemainingTime() - deltaTime);

            if(currentTask->getRemainingTime() <= 0){
                // calcula o tempo de fim da tarefa
                currentTask->setEndTime(globalClock);

                updateTask(currentTask);

                int id = currentTask->getId();

                // remove a tarefa na fila de prontas do simulator
                removeTask(id);

                // remove a tarefa na fila de prontas do escalonador
                scheduler->removeTask(id);
            }

            currentTask = scheduler->getNextTask();
        }
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

const bool Simulator::existId(int id)
{
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        if(tasks[i]->getId() == id)
            return true;
    
    return false;
}

int Simulator::modifyId(int id)
{
    static std::mt19937 gen(std::random_device{}());

    int newId = id;
    int factor = 2;
    const int maxFactor = 1000000; // seguranca
    int numAttempts = 0;
    
    // metodo escolhido: dou preferencia aos numeros menores
    while(existId(newId)){
        std::uniform_int_distribution<int> dist(0, factor - 1);
        newId = dist(gen);

        numAttempts++;

        if(numAttempts >= factor / 2 && factor < maxFactor)
            factor *= 2;
    }

    return newId;
}

// Usado no depurador
// Cria uma tarefa, com as informacoes do idTask passado, no heap
void Simulator::createTask(std::vector<TCB*>& pTasks, int idTask)
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

void Simulator::deleteTasks(std::vector<TCB*>& pTasks)
{
    size_t tam = pTasks.size();
    for(size_t i = 0; i < tam; i++){
        if(pTasks[i] != nullptr){
            delete pTasks[i];
            pTasks[i] = nullptr;
        }
    }

    pTasks.clear();
}

void Simulator::chosenMode(const std::vector<TCB*>& pTasks, const int& currentIdTask, const unsigned int& globalClock)
{
    menu->clearTerminal();

    showMinimumInfo(currentIdTask, globalClock);

    std::cout << 
        "Enter the number of the desired option:\n\n" <<
        "1. See all information for each task;\n" <<
        "2. View information about tasks in the ready queue, and information about the executed task;\n" <<
        "3. Proceed to the next instant of time;\n" <<
    std::endl;

    int option = menu->checkEntryNumber(1, 3);

    if(option == 1){
        showAllTasks(pTasks, currentIdTask, globalClock);
    }
    else if(option == 2){
        showReadyTasks(pTasks, currentIdTask, globalClock);
    }

    menu->clearTerminal();
}

void Simulator::showMinimumInfo(const int& currentIdTask, const unsigned int& globalClock)
{
    std::cout << "Current time instant: " << globalClock << "\n" << "Executed Task ID: " << currentIdTask << "\n" << std::endl;
}

void Simulator::showAllTasks(const std::vector<TCB*>& pTasks, const int& currentIdTask, const unsigned int& globalClock)
{
    menu->clearTerminal();

    showMinimumInfo(currentIdTask, globalClock);

    // Mostra as informacoes das tarefas para o usuario
    std::cout << "Tasks information:\n" << std::endl;
    size_t tam = pTasks.size();
    for(size_t i = 0; i < tam; i++){
        std::cout << 
        "ID: " << pTasks[i]->getId() << "\n" <<
        "Color: " << pTasks[i]->getColor() << "\n" <<
        "Entry time: " << pTasks[i]->getEntryTime() << "\n" <<
        "Duration: " << pTasks[i]->getDuration() << "\n" <<
        "Priority: " << pTasks[i]->getPriority() << "\n" <<
        "Remaining time: " << pTasks[i]->getRemainingTime() << "\n";

        if(pTasks[i]->getEndTime() != std::numeric_limits<unsigned int>::max())
            std::cout << "End time: " << pTasks[i]->getEndTime() << "\n";

        std::cout << std::endl;
    }

    std::cout << "Press enter to return to the options menu" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cout
    std::cin.get();

    chosenMode(pTasks, currentIdTask, globalClock);

    menu->clearTerminal();
}

void Simulator::showReadyTasks(const std::vector<TCB*>& pTasks, const int &currentIdTask, const unsigned int &globalClock)
{
    menu->clearTerminal();

    showMinimumInfo(currentIdTask, globalClock);

    // Mostra as informacoes das tarefas para o usuario
    std::cout << "Information about tasks in the ready queue, and the executed task:\n" << std::endl;

    std::vector<TCB>& t = scheduler->getTasks();
    size_t tam = scheduler->getTasks().size();
    for(size_t i = 0; i < tam; i++){
        std::cout << 
        "ID: " << t[i].getId() << "\n" <<
        "Color: " << t[i].getColor() << "\n" <<
        "Entry time: " << t[i].getEntryTime() << "\n" <<
        "Duration: " << t[i].getDuration() << "\n" <<
        "Priority: " << t[i].getPriority() << "\n" <<
        "Remaining time: " << t[i].getRemainingTime() << "\n";

        std::cout << std::endl;
    }

    std::cout << "Press enter to return to the options menu" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cout
    std::cin.get();

    chosenMode(pTasks, currentIdTask, globalClock);

    menu->clearTerminal();
}

std::vector<TCB*> Simulator::loadArquive() {
    std::ifstream file("configuration.txt");
    
    if (!file.is_open()) {
        std::cerr << "Error opening configuration file" << std::endl;
        return std::vector<TCB*>();
    }

    std::string line;

    // 1) Ler primeira linha inteira: algoritmo;quantum
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

    if (!std::getline(ss, alg, ';')) {
        std::cerr << "Malformed first line: missing algorithm" << std::endl;
        return std::vector<TCB*>();
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
            return std::vector<TCB*>();
        }
    }
    remove_cr(quantumStr);
    trim(quantumStr);
    if (quantumStr.empty()) {
        std::cerr << "Quantum empty in configuration" << std::endl;
        return std::vector<TCB*>();
    }
    try {
        extraInfo.setQuantum(std::stoi(quantumStr));
    } catch (const std::exception &e) {
        std::cerr << "Invalid quantum value: '" << quantumStr << "'" << std::endl;
        return std::vector<TCB*>();
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

        // verifica se o ID ja existe
        if(existId(tmp)){
            // muda o valor dele
            int prev = tmp;
            tmp = modifyId(tmp);

            std::cout << "The task ID " << prev << " was modified for " << tmp << ", because " << prev << " already exists.\n" << std::endl;
        }

        task->setId(tmp);
        
        // Color
        if (!safeGetInt(tmp)) { 
            std::cerr << "Bad or missing Color in line: " << line << std::endl; 
            continue; 
        }
        task->setColor(tmp);
        
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
        task->setPriority(tmp);

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

void Simulator::removeTask(unsigned int idTask)
{
    std::vector<TCB*>::iterator it;
    for(it = tasks.begin(); (*it)->getId() != idTask; it++);

    // Remove a tarefa original do simulador
    tasks.erase(it);
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

// timeNow em ticks
const bool Simulator::canAnyTasksEnter(double timeNow, std::vector<unsigned int>& indexTasks, const std::vector<int>& exceptionIdTasks)
{
    bool found = false;

    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++){
        // verifica se o id de tasks esta dentro de exceptionIdTasks
        std::vector<int>::const_iterator it;
        it = std::find(exceptionIdTasks.begin(), exceptionIdTasks.end(), tasks[i]->getId());

        if(it == exceptionIdTasks.end() && timeNow >= tasks[i]->getEntryTime()){
            indexTasks.push_back(static_cast<unsigned int>(i));
            found = true;
        }
    }
    
    return found;
}
