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
    // calcular o tick/segundo
    int tps = calcTicksPerSecond();

    std::chrono::duration<double> globalClock = std::chrono::seconds(0);
    auto last = std::chrono::steady_clock::now();

    while(globalClock.count() < scheduler->getSumDurationTasks()){ 
        auto now = std::chrono::steady_clock::now();

        globalClock += last - now;
        last = now;
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
    std::vector<TCB> tasks;
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

    scheduler->setTasks(tasks);

    arquive.close();

    return tasks;
}

void Simulator::addTask(TCB task)
{
    scheduler->addTask(task);
}

void Simulator::setAlgorithmScheduler(int i)
{
    if(i == 1){
        extraInfo.setAlgorithmScheduler("FIFO");

        FIFO* algo = new FIFO();

        scheduler = static_cast<Scheduler*>(algo);
    }
    else if(i == 2){
        extraInfo.setAlgorithmScheduler("SRTF");

        SRTF* algo = new SRTF();

        scheduler = static_cast<Scheduler*>(algo);
    }
    else if(i == 3){
        extraInfo.setAlgorithmScheduler("PRIOp");

        PreemptivePriority* algo = new PreemptivePriority();

        scheduler = static_cast<Scheduler*>(algo);
    }
}

void Simulator::setAlgorithmScheduler(std::string algorithm)
{
    extraInfo.setAlgorithmScheduler(algorithm);

    if(algorithm == "FIFO"){
        FIFO* algo = new FIFO();

        scheduler = static_cast<Scheduler*>(algo);
    }
    else if(algorithm == "SRTF"){
        SRTF* algo = new SRTF();

        scheduler = static_cast<Scheduler*>(algo);
    }
    else if(algorithm == "PRIOp"){
        PreemptivePriority* algo = new PreemptivePriority();

        scheduler = static_cast<Scheduler*>(algo);
    }
}

std::vector<TCB> Simulator::getTasks() const
{
    return scheduler->getTasks();
}

std::string Simulator::getAlgorithmScheduler() const
{
    return extraInfo.getAlgorithmScheduler();
}

unsigned int Simulator::getQuantum() const
{
    return extraInfo.getQuantum();
}

unsigned int Simulator::calcTicksPerSecond()
{
    std::vector<TCB> tasks = scheduler->getTasks();

    unsigned int maxBeginTime = 0;
    size_t tam = tasks.size();
    for(size_t i = 0; i < tam; i++)
        if(tasks[i].getEntryTime() > maxBeginTime)
            maxBeginTime = tasks[i].getEntryTime();

    int durationSimulator = 10; // 10 seconds
    return (maxBeginTime / durationSimulator);
}
