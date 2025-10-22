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

    TCB currentTask;
    currentTask.setId(INT_MIN);
    unsigned int timeLastInterrupt = 0; // in ticks

    // calcula o tick/segundo
    double tps = calcTicksPerSecond();

    // cria o relogio global simulado
    const double deltaTime = 1.0 / tps;
    double globalClock = 0.0;

    // enquanto houver tarefas no simulador (na "memoria")
    while(tasks.size()){ 
        // atualiza relogio
        globalClock += deltaTime;

        // verifica se o tempo atual corresponde a alguma tarefa que pode entrar no escalonador
        unsigned int indexTask = 0;
        if(canAnyTaskEnter(globalClock * tps, &indexTask, currentTask.getId())){

            // ignora a primeira interrupcao
            if(currentTask.getId() != INT_MIN){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask.getId(), currentTask.getColor(), globalClock * tps, timeLastInterrupt);
            }

            timeLastInterrupt = globalClock * tps;

            // adiciona a tarefa na fila de prontas do escalonador
            scheduler->addTask(tasks[indexTask]);

            // "executa" a tarefa no processador
            currentTask = scheduler->getNextTask();
        }

        // Verifica se o tempo restante da tarefa executada acabou 
        if(currentTask.getId() != INT_MIN){
            currentTask.setRemainingTime(currentTask.getDuration() - (globalClock * tps - currentTask.getEntryTime()));

            if(currentTask.getRemainingTime() <= 0){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask.getId(), currentTask.getColor(), globalClock * tps, timeLastInterrupt);

                timeLastInterrupt = globalClock * tps;

                // remove a tarefa na fila de prontas do simulator
                removeTask(currentTask.getId());

                // remove a tarefa na fila de prontas do escalonador
                scheduler->removeTask(currentTask.getId());

                // "executa" outra tarefa no processador
                currentTask = scheduler->getNextTask();
            }
        }
    }

    generateImage();
}

std::vector<TCB> Simulator::loadArquive()
{
    std::ifstream arquive("assets/configuration.txt");

    if (!arquive.is_open()) {
        std::cerr << "Error for open arquive configuration" << std::endl;
        return std::vector<TCB>();
    }

    // Consegue o algoritmo
    std::string word;
    std::getline(arquive, word, ';');

    setAlgorithmScheduler(word);

    // Consegue o quantum
    std::getline(arquive, word, ';');
    extraInfo.setQuantum(std::stoi(word));

    // Consegue as configuracoes das tarefas
    std::string line;
    while(std::getline(arquive, line)){
        TCB task;
        std::istringstream lineStream(line);

        // Consegue o ID
        std::getline(lineStream, word, ';');
        task.setId(std::stoi(word));

        // Consegue a cor
        std::getline(lineStream, word, ';');
        task.setColor(std::stoi(word));

        // Consegue o instante de ingresso
        std::getline(lineStream, word, ';');
        task.setEntryTime(std::stoi(word));

        // Consegue a duracao
        std::getline(lineStream, word, ';');
        task.setDuration(std::stoi(word));

        // Consegue a prioridade
        std::getline(lineStream, word, ';');
        task.setPriority(std::stoi(word));

        tasks.push_back(task);
    }

    arquive.close();

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
    for(it = tasks.begin(); it->getId() == idTask; it++);
    
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
const bool Simulator::canAnyTaskEnter(double timeNow, unsigned int* indexTask, const unsigned int& exceptionIdTask)
{
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++){
        if(tasks[i].getId() != exceptionIdTask && timeNow >= tasks[i].getEntryTime()){
            (*indexTask) = i;
            return true;
        }
    }
    
    return false;
}
