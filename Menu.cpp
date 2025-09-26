#include "Menu.h"
#include "Simulator.h"

const std::string Menu::inicialText = R"(
    Welcome to scheduler simulator!
 
    Clique em qualquer tecla para comecar.
)";

const std::string Menu::arquiveText = R"(
    Carregar arquivo texto?

    Para comecar, eh necessario configurar o simulador, para isso, escolha uma das opcoes abaixo:

    1 -> Carregar a partir do arquivo plain.txt
    2 -> Configurar do zero

    Digite a opcao (o numero) desejada:
)";

const std::string Menu::chosenModeText = R"(
    Escolha o modo de execucao:

    1 -> Passo a passo (Debugger)
    2 -> Direto

    Digite a opcao (o numero) desejada:
)";

const std::string Menu::algorithmText = R"(
    Escolha um algoritmo para o escalonador:

    1 -> FIFO (First In First Out)
    2 -> SRTF (Shortest Remaining Time First)
    3 -> Prioridade preemptiva

    Digite a opcao (o numero) desejada:
)";

const std::string Menu::taskText = R"(
    Criar nova tarefa?
    
    Numero minimo de tarefas para funcionar o simulador: 1
)";

const std::string Menu::colorText = R"(
    Selecione a cor da nova tarefa:

    1 -> Vermelho
    2 -> Verde
    3 -> Amarelo
    4 -> Azul
    5 -> Magenta
    6 -> Ciano
    7 -> Branco

    Digite a opcao (o numero) desejada:
)";

Menu::Menu() : simulator(nullptr)
{
}

Menu::~Menu(){
    if(simulator != nullptr){
        delete simulator;
        simulator = nullptr;
    }
}

void Menu::execute()
{
    createInicialScreen();

    createArquiveScreen();

    createChosenModeScreen();
}

void Menu::createInicialScreen()
{
    clearTerminal();

    std::cout << inicialText << std::endl;
    std::cin.get();

    clearTerminal();
}

void Menu::createArquiveScreen()
{
    clearTerminal();

    std::cout << arquiveText << std::endl;

    int option = checkEntryNumber(1, 2);

    clearTerminal();

    if(option == 1){
        createConfirmationScreen(simulator->loadArquive());
    }
    else if(option == 2){
        createAlgorithmScreen();
        createTaskScreen();
        createConfirmationScreen(simulator->getTasks());
    }

    clearTerminal();
}

void Menu::createConfirmationScreen(std::vector<TCB> tasks)
{
    clearTerminal();

    std::cout << "Confirme as configuracoes:\n" << std::endl;

    std::cout << "Algoritmo do escalonador: " << simulator->getAlgorithmScheduler() << std::endl;
    std::cout << "Duracao do quantum: " << simulator->getQuantum() << "\n" << std::endl;

    std::cout << "Tarefas:\n" << std::endl;

    // Todas as informacoes de cada uma das tarefas sao mostradas no terminal 
    size_t tam = tasks.size();
    for(int i = 0; i < tam; i++){
        std::cout << 
            "ID: " << tasks[i].getId() << "\n" <<
            "Color: " << tasks[i].getColor() << "\n" <<
            "Duration: " << tasks[i].getDuration() << "\n" <<
            "Priority: " << tasks[i].getPriority() << "\n" <<
            "Entry time: " << tasks[i].getEntryTime() << "\n" <<
        std::endl;
    }

    std::cout << "Pressione qualquer tecla para executar o simulador." << std::endl;
    std::cin.get();

    clearTerminal();
}

void Menu::createChosenModeScreen()
{
    clearTerminal();

    std::cout << chosenModeText << std::endl;
    int option = checkEntryNumber(1, 2);

    clearTerminal();

    if(option == 1){
        simulator->executeDebugger();
    }
    else if(option == 2){
        simulator->executeNoDebugger();
    }

    clearTerminal();
}

void Menu::createAlgorithmScreen()
{
    clearTerminal();

    std::cout << algorithmText << std::endl;

    simulator->setAlgorithmScheduler(checkEntryNumber(1, 3));

    clearTerminal();
}

void Menu::createTaskScreen()
{
    clearTerminal();

    std::cout << taskText << std::endl;

    int numTasks = 0;
    std::string option = "Y";

    while(option == "Y" || option == "y"){
        std::cout 
            << "Numero atual de tarefas: " << numTasks << "\n\n"
            << "Deseja criar uma nova tarefa? (Y ou N)" << std::endl;
        
        std::vector<std::string> targets;
        targets.push_back("Y");
        targets.push_back("y");
        targets.push_back("N");
        targets.push_back("n");

        option = checkEntryString(targets);

        if(option == "N" || option == "n") break;

        TCB task;
        int num;

        task.setId(numTasks);

        std::cout << "\nID da tarefa: " << numTasks << std::endl;
        std::cout << colorText << std::endl;

        task.setColor(checkEntryNumber(1, 7));

        std::cout << "\nDigite o instante do tempo que a tarefa entra no simulador:" << std::endl;
        std::cin >> num;

        task.setEntryTime(num);

        std::cout << "\nDigite a duracao da tarefa:" << std::endl;
        std::cin >> num;

        task.setDuration(num);

        std::cout << "\nDigite a prioridade da tarefa:" << std::endl;
        std::cin >> num;

        task.setPriority(num);

        numTasks++;
        simulator->addTask(task);

        clearTerminal();
    }

    clearTerminal();
}

// Dada uma sequencia de opcoes, verifica se a entrada do usuario eh valida,
// e retorna a opcao escolhida
int Menu::checkEntryNumber(int firstOption, int lastOption)
{
    int option;
    std::cin >> option;

    while(option < firstOption || option > lastOption){
        std::cout << "\nOpcao invalida. Tente novamente.\n" << std::endl;
        std::cin >> option;
    }

    return option;
}

// Dada um conjunto de strings desejadas, verifica se a entrada do usuario eh valida
// Retorna a string que coincidiu com uma do conjunto
std::string Menu::checkEntryString(std::vector<std::string> targets)
{
    std::string str;

    while (true) {
        std::cin >> str;

        // verifica se str existe no vetor
        if (std::find(targets.begin(), targets.end(), str) != targets.end()) {
            return str; // achou, retorna
        }

        std::cout << "\nOpcao invalida. Tente novamente.\n" << std::endl;
    }
}

void Menu::clearTerminal()
{
#ifdef _WIN32
	system("cls");  // Windows
#else
	system("clear");  // Linux/macOS
#endif
}