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

    imageGenerator->createAxis(tasks.size(), getIdTasks(), sumDurationTasks());

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

    // enquanto houver tarefas no simulador (na "memoria")
    while(tasks.size()){ 
        auto now = std::chrono::steady_clock::now();

        // atualiza relogio
        globalClock += last - now;
        last = now;

        // verifica se o tempo atual corresponde a tarefa que entra por primeiro no escalonador
        if(globalClock.count() * tps <= tasks[0].getEntryTime()){

            // desenha na imagem o que aconteceu no processador ate agora (interrupcao)
            // em base na tarefa atual
            imageGenerator->addRectTask(currentTask);

            // adiciona a tarefa na fila de prontas do escalonador
            scheduler->addTask(tasks[0]);

            // remove a tarefa da memoria
            tasks.erase(tasks.begin());

            // "executa" a tarefa no processador
            currentTask = scheduler->getNextTask();
        }
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
