#include "Menu.h"
#include "Simulator.h"

const std::string Menu::inicialText = R"(
Welcome to scheduler simulator!
 
Press any key to start.
)";

const std::string Menu::arquiveText = R"(
Load the text file?

To begin, you need to configure the simulator. To do so, choose one of the options below:

1 -> Load from the configuration.txt file
2 -> Configure from scratch

Enter the desired option (number):
)";

const std::string Menu::chosenModeText = R"(
Choose the execution mode:

1 -> Step by Step (Debugger)
2 -> Direct

Enter the desired option (number):
)";

const std::string Menu::algorithmText = R"(
Choose an algorithm for the scheduler:

1 -> FIFO (First In First Out)
2 -> SRTF (Shortest Remaining Time First)
3 -> PRIOp (Priority Preemption)

Enter the desired option (number):
)";

const std::string Menu::taskText = R"(
Create a new task?

Minimum number of tasks for the simulator to work: 1
)";

const std::string Menu::colorText = R"(
Select the color of the new task:

1 -> Red
2 -> Green
3 -> Yellow
4 -> Blue
5 -> Magenta
6 -> Cyan

Enter the desired option (number):
)";

const std::string Menu::quantumText = R"(
What is the value of the quantum?
)";

Menu::Menu() : simulator(nullptr)
{
}

Menu::~Menu(){
    simulator = nullptr;
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
        createQuantumScreen();
        createTaskScreen();
        createConfirmationScreen(simulator->getTasks());
    }

    clearTerminal();
}

void Menu::createConfirmationScreen(const std::vector<TCB*>& tasks)
{
    clearTerminal();

    std::cout << "Confirm the settings:\n" << std::endl;

    std::cout << "Scheduler algorithm: " << simulator->getAlgorithmScheduler() << std::endl;
    std::cout << "Quantum duration: " << simulator->getQuantum() << "\n" << std::endl;

    std::cout << "Tasks:\n" << std::endl;

    // Todas as informacoes de cada uma das tarefas sao mostradas no terminal 
    size_t tam = tasks.size();
    for(int i = 0; i < tam; i++){
        std::cout << 
            "ID: " << tasks[i]->getId() << "\n" <<
            "Color: " << tasks[i]->getColor() << "\n" <<
            "Entry time: " << tasks[i]->getEntryTime() << "\n" <<
            "Duration: " << tasks[i]->getDuration() << "\n" <<
            "Priority: " << tasks[i]->getPriority() << "\n" <<
        std::endl;
    }

    std::flush(std::cout);

    std::cout << "Press any key to execute the simulator." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cout
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

    std::cout << "Simulator executed successfully. \nOpen the image.svg file to view the resulting graph." << std::endl;
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
            << "Current number of tasks: " << numTasks << "\n\n"
            << "Do you want to create a new task? (Y or N)" << std::endl;
        
        std::vector<std::string> targets;
        targets.push_back("Y");
        targets.push_back("y");
        targets.push_back("N");
        targets.push_back("n");

        option = checkEntryString(targets);

        if(option == "N" || option == "n"){
            if(numTasks == 0){
                option = "Y";

                std::cout << "\nIt's not possible to iniciate the simulator with 0 (zero) tasks.\n" << std::endl;
                std::cin.get();
                
                clearTerminal();
                continue;
            }
            else{
                break;
            }
        }

        clearTerminal();

        std::cout 
            << "Do you want to create a standard task? (Y or N)\n\n"
            << "If yes, then the new task will have the following settings:\n\n"
            << "ID: " << numTasks << "\n"
            << "Color: 1 (red)\n"
            << "Entry time: 1\n"
            << "Duration: 1\n"
            << "Priority: 1\n" <<
        std::endl;

        std::string res = checkEntryString(targets);

        if(res == "Y" || res == "y"){
            TCB task;
            task.setId(numTasks);
            task.setColor(1);
            task.setEntryTime(1);
            task.setDuration(1);
            task.setPriority(1);

            numTasks++;
            simulator->addTask(task);
        }
        else if(res == "N" || res == "n"){
            clearTerminal();

            TCB task;
            int num;

            task.setId(numTasks);

            std::cout << "\nTask ID: " << numTasks << std::endl;
            std::cout << colorText << std::endl;

            task.setColor(checkEntryNumber(1, 6));

            std::cout << "\nEnter the time instant that the task enters the simulator:" << std::endl;
            std::cin >> num;

            task.setEntryTime(num);

            std::cout << "\nEnter the task duration:" << std::endl;
            std::cin >> num;

            task.setDuration(num);

            std::cout << "\nEnter the task priority:" << std::endl;
            std::cin >> num;

            task.setPriority(num);

            numTasks++;
            simulator->addTask(task);
        }

        clearTerminal();
    }

    clearTerminal();
}

void Menu::createQuantumScreen()
{
    clearTerminal();

    std::cout << quantumText << std::endl;

    simulator->setQuantum(checkEntryNumber((unsigned int)0, (unsigned int)4294967294));

    clearTerminal();
}

// Dada uma sequencia de opcoes, verifica se a entrada do usuario eh valida,
// e retorna a opcao escolhida
int Menu::checkEntryNumber(int firstOption, int lastOption)
{
    int option;
    std::cin >> option;

    while(option < firstOption || option > lastOption){
        std::cout << "\nInvalid option. Try again.\n" << std::endl;
        std::cin >> option;
    }

    return option;
}

// Dada um intervalo de numeros, verifica se a entrada do usuario eh valida,
// e retorna o numero escolhido
unsigned int Menu::checkEntryNumber(unsigned int firstNumber, unsigned int lastOption)
{
    unsigned int num;
    std::cin >> num;

    while(num < firstNumber || num > lastOption){
        std::cout << "\nInvalid option. Try again.\n" << std::endl;
        std::cin >> num;
    }

    return num;
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

        std::cout << "\nInvalid option. Try again.\n" << std::endl;
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

void Menu::setSimulator(Simulator *s)
{
    if(s != nullptr)
        simulator = s;
}
