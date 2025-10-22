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
    menu->execute();
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

    // calcula o tick/segundo
    double tps = calcTicksPerSecond();

    // cria o relogio global
    std::chrono::duration<double> globalClock = std::chrono::seconds(0);
    auto last = std::chrono::steady_clock::now();

    TCB currentTask;
    currentTask.setId(INT_MIN);
    unsigned int timeLastInterrupt = 0; // in ticks

    // enquanto houver tarefas no simulador (na "memoria")
    while(tasks.size()){ 
        auto now = std::chrono::steady_clock::now();

        // atualiza relogio
        globalClock += now - last;
        last = now;

        // verifica se o tempo atual corresponde a alguma tarefa que pode entrar no escalonador
        unsigned int indexTask = 0;
        if(canAnyTaskEnter(globalClock.count() * tps, &indexTask)){

            // ignora a primeira interrupcao
            if(currentTask.getId() != INT_MIN){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask.getId(), currentTask.getColor(), globalClock.count() * tps, timeLastInterrupt);
            }

            timeLastInterrupt = globalClock.count() * tps;

            // adiciona a tarefa na fila de prontas do escalonador
            scheduler->addTask(tasks[indexTask]);

            // "executa" a tarefa no processador
            currentTask = scheduler->getNextTask();
        }

        // Verifica se o tempo restante da tarefa executada acabou 
        if(currentTask.getId() != INT_MIN){
            currentTask.setRemainingTime(currentTask.getDuration() - (globalClock.count() * tps - currentTask.getEntryTime()));

            if(currentTask.getRemainingTime() <= 0){
                // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
                // em base na tarefa atual
                imageGenerator->addRectTask(currentTask.getId(), currentTask.getColor(), globalClock.count() * tps, timeLastInterrupt);

                timeLastInterrupt = globalClock.count() * tps;

                // remove a tarefa na fila de prontas do simulator
                removeTask(currentTask.getId());

                // remove a tarefa na fila de prontas do escalonador
                scheduler->removeTask(currentTask.getId());

                // "executa" outra tarefa no processador
                currentTask = scheduler->getNextTask();
            }
        }
    }

    // Desenha a ultima tarefa
    if(currentTask.getId() != INT_MIN){
        // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
        // em base na tarefa atual
        imageGenerator->addRectTask(currentTask.getId(), currentTask.getColor(), globalClock.count() * tps, timeLastInterrupt);
    }
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
    unsigned int maxBeginTime = 0;
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        if(tasks[i].getEntryTime() > maxBeginTime)
            maxBeginTime = tasks[i].getEntryTime();

    double durationSimulator = 10.f; // 10 seconds
    return ((double)maxBeginTime / durationSimulator);
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
const bool Simulator::canAnyTaskEnter(double timeNow, unsigned int* numTask)
{
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++){
        if(timeNow >= tasks[i].getEntryTime()){
            (*numTask) = i;
            return true;
        }
    }
    
    return false;
}
