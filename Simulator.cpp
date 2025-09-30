#include "Simulator.h"
#include "Menu.h"

Simulator::Simulator() :
menu(nullptr),
scheduler(nullptr),
imageGenerator(nullptr),
extraInfo(nullptr){

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

    if(extraInfo != nullptr){
        delete extraInfo;
        extraInfo = nullptr;
    }
}

void Simulator::start()
{
    menu = new Menu();
    menu->execute();
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
    int quantum = std::stoi(word);

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

    arquive.close();

    return tasks;
}

void Simulator::setAlgorithmScheduler(int i)
{
    if(i == 1){
        FIFO* algo = new FIFO();

        scheduler = static_cast<Scheduler*>(algo);
    }
    else if(i == 2){
        SRTF* algo = new SRTF();

        scheduler = static_cast<Scheduler*>(algo);
    }
    else if(i == 3){
        PreemptivePriority* algo = new PreemptivePriority();

        scheduler = static_cast<Scheduler*>(algo);
    }
}

void Simulator::setAlgorithmScheduler(std::string algorithm)
{
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

std::string Simulator::getAlgorithmScheduler() const
{
    return extraInfo->getAlgorithmScheduler();
}

unsigned int Simulator::getQuantum() const
{
    return extraInfo->getQuantum();
}
