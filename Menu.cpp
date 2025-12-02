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

1 -> FIFO (First In First Out), FCFS (First Come First Served) or RR (Round-Robin). Depends on the quantum.
2 -> SRTF (Shortest Remaining Time First)
3 -> PRIOp (Priority Preemption)
4 -> PRIOPEnv (Priority Preemption with aging)

Enter the desired option (number):
)";

const std::string Menu::taskText = R"(
Create a new task?

Minimum number of tasks for the simulator to work: 1
)";

const std::string Menu::alphaText = R"(
What is the value of the alpha?
)";

const std::string Menu::colorText = R"(
Select or type a hexadecimal RGB color for the new task:

Standard colors:

1 -> Red
2 -> Green
3 -> Yellow
4 -> Blue
5 -> Magenta
6 -> Cyan
)";

const std::string Menu::quantumText = R"(
What is the value of the quantum? 
If zero, the quantum will not interfere with the simulator.
)";

const std::string Menu::editText = R"(
What setting do you want change?

1 -> Scheduler algorithm
2 -> Quantum value
3 -> Alpha value
4 -> Tasks information

Enter the desired option (number):
)";

const std::string Menu::editTaskText = R"(
What task information do you want change?

1 -> ID
2 -> Color
3 -> Entry time
4 -> Duration
5 -> Priority
6 -> IO operations
7 -> Mutexes action

Enter the desired option (number):
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

    int option = checkEntryNumber((unsigned int)1, (unsigned int)2);

    clearTerminal();

    if(option == 1){
        createConfirmationScreen(simulator->loadArquive());
    }
    else if(option == 2){
        createAlgorithmScreen();
        createQuantumScreen();
        
        if(simulator->getAlgorithmScheduler() == "PRIOPEnv")
            createAlphaScreen();

        createTaskScreen();
        createConfirmationScreen(simulator->getTasks());
    }

    clearTerminal();
}

void Menu::createConfirmationScreen(const std::vector<TCB*>& tasks)
{
    std::cout << "Confirm the settings:\n" << std::endl;

    std::cout << "Scheduler algorithm: " << simulator->getAlgorithmScheduler() << std::endl;
    std::cout << "Quantum duration: " << simulator->getQuantum() << std::endl;
    std::cout << "Alpha value: " << simulator->getAlpha() << "\n" << std::endl;

    // Todas as informacoes de cada uma das tarefas sao mostradas no terminal 
    showTasks(tasks);

    std::flush(std::cout);

    std::cout << "Choose an option:\n\n"
              << "1 -> Add more tasks\n"
              << "2 -> Edit the settings\n"
              << "3 -> Confirm the settings\n" << std::endl;
    
    unsigned int option = checkEntryNumber((unsigned int)1, (unsigned int)3);
    if(option == 1){
        createTaskScreen();
        createConfirmationScreen(simulator->getTasks());
    }
    else if(option == 2){
        createEditScreen();
        createConfirmationScreen(simulator->getTasks());
    }

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

    simulator->setAlgorithmScheduler(checkEntryNumber((unsigned int)1, (unsigned int)4));

    clearTerminal();
}

void Menu::createTaskScreen()
{
    clearTerminal();

    std::cout << taskText << std::endl;

    int numTasks = simulator->getTasks().size();
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

                std::cout 
                    << "\nIt's not possible to iniciate the simulator with 0 (zero) tasks.\n" 
                    << "Press Enter to continue.\n" <<
                std::endl;

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cin
                std::cin.get();
                
                clearTerminal();
                continue;
            }
            else{
                break;
            }
        }

        clearTerminal();

        // Ocorre na etapa de edicao de tarefas
        unsigned int standardId = numTasks;
        while(simulator->existTaskId(standardId))
            standardId = simulator->modifyId(standardId);

        std::cout 
            << "Do you want to create a standard task? (Y or N)\n\n"
            << "If yes, then the new task will have the following settings:\n\n"
            << "ID: " << standardId << "\n"
            << "Color: 1 (red)\n"
            << "Entry time: 1\n"
            << "Duration: 1\n"
            << "Priority: 1\n"
            << "No I/O operations\n"
            << "No mutex actions\n" <<
        std::endl;

        std::string res = checkEntryString(targets);

        if(res == "Y" || res == "y"){
            TCB task;
            task.setId(standardId);
            task.setColor(1);
            task.setEntryTime(1);
            task.setDuration(1);
            task.setStaticPriority(1);

            numTasks++;
            simulator->addTask(task);
        }
        else if(res == "N" || res == "n"){
            clearTerminal();

            TCB task;
            unsigned int num;

            task.setId(numTasks);

            std::cout << "\nTask ID: " << numTasks << std::endl;
            std::cout << colorText << std::endl;

            // var pode assumir um dos dois possiveis valores (unsigned int ou string)
            std::variant<unsigned int, std::string> var = checkEntryColor((unsigned int)1, (unsigned int)6);

            // se var for unsigned int
            if(std::holds_alternative<unsigned int>(var))
                task.setColor(std::get<unsigned int>(var));
            // se var for string
            else if(std::holds_alternative<std::string>(var))
                task.setColor(std::get<std::string>(var));

            std::cout << "\nEnter the time instant that the task enters the simulator:" << std::endl;
            num = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

            task.setEntryTime(num);

            std::cout << "\nEnter the task duration:" << std::endl;
            num = checkEntryNumber((unsigned int)1, std::numeric_limits<unsigned int>::max());

            task.setDuration(num);

            std::cout << "\nEnter the task priority:" << std::endl;
            num = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

            task.setStaticPriority(num);

            // Operacoes I/O
            res = "Y";
            while(res == "Y" || res == "y"){
                std::cout << "\nDo you want to add an IO operation? (Y or N)" << std::endl;
                res = checkEntryString(std::vector<std::string>{"Y", "y", "N", "n"});

                if(res == "N" || res == "n") break;

                unsigned int IO_InitialTime = 0;
                unsigned int IO_duration = 0;

                std::cout 
                    << "\nEnter the IO operation initial time (relative to task entry time):" 
                    << "\nThe value cannot be bigger to task duration."
                    << std::endl;
                IO_InitialTime = checkEntryNumber((unsigned int)0, (unsigned int)task.getDuration());

                std::cout 
                    << "\nEnter the IO operation duration time:"
                    << std::endl;
                IO_duration = checkEntryNumber((unsigned int)1, std::numeric_limits<unsigned int>::max());

                if(!task.addIO_operation(std::pair<unsigned int, unsigned>{IO_InitialTime, IO_duration})){
                    std::cout 
                        << "The I/O operation cannot be inserted.\n" 
                        << "Press any key to recreate the I/O operation again.\n"
                        << std::endl;
                    std::cin.get();
                }
            }

            // mutex action
            // para cada resposta positiva pelo usuario
            // eh criado dois mutex action (para impedir inconsistencias)
            // no preExecuteDefault, de simulator.h, eh criado os objetos Mutex
            res = "Y";
            while(res == "Y" || res == "y"){
                std::cout << "\nDo you want to add an mutex for the task? (Y or N)" << std::endl;
                res = checkEntryString(std::vector<std::string>{"Y", "y", "N", "n"});

                if(res == "N" || res == "n") break;

                MutexAction mutexActionLock, mutexActionUnlock;

                // tipo
                mutexActionLock.setType(MutexAction::Type::Lock);
                mutexActionUnlock.setType(MutexAction::Type::Unlock);

                // ID
                std::cout << "\nEnter the mutex ID:" << std::endl;
                unsigned int mutexActionId = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

                mutexActionLock.setId(mutexActionId);
                mutexActionUnlock.setId(mutexActionId);

                // tempo
                std::cout << "\nEnter the mutex initial time (relative to task):" << std::endl;
                mutexActionLock.setTime(checkEntryNumber((unsigned int)0, task.getDuration()));

                std::cout << "\nEnter the mutex end time (relative to task):" << std::endl;
                mutexActionUnlock.setTime(checkEntryNumber((unsigned int)mutexActionLock.getTime() + 1, task.getDuration()));

                // verifica casos como ML01:02 MU01:04 ML01:00 MU01:04
                if (!task.canAddMutexesAction(mutexActionId,
                            mutexActionLock.getTime(),
                            mutexActionUnlock.getTime()))
                {
                    std::cout << "\nERROR: Invalid mutex action sequence for mutex ID "
                            << mutexActionId << ".\n";
                    std::cout << "This may be caused by:\n"
                            << "- Unlock before lock\n"
                            << "- Lock before previous unlock\n"
                            << "- Overlapping intervals\n";
                    continue; // não adiciona
                }
                                
                task.addMutexAction(mutexActionLock);
                task.addMutexAction(mutexActionUnlock);
            }

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

    simulator->setQuantum(checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max()));

    clearTerminal();
}

void Menu::createEditScreen()
{
    clearTerminal();

    std::cout << editText << std::endl;

    unsigned int option = checkEntryNumber((unsigned int)1, (unsigned int)4);

    clearTerminal();

    if(option == 1){
        std::cout << "Previous algorithm scheduler: " << simulator->getAlgorithmScheduler() << std::endl;
        std::cout << algorithmText << std::endl;

        simulator->setAlgorithmScheduler(checkEntryNumber((unsigned int)1, (unsigned int)4));
    }
    else if(option == 2){
        std::cout << "Previous value of the quantum: " << simulator->getQuantum() << std::endl;
        std::cout 
            << "\nWhat is the value of the new quantum?" 
            << "\nIf zero, the quantum will not interfere with the simulator."
            << std::endl;

        simulator->setQuantum(checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max()));
    }
    else if(option == 3){
        std::cout << "Previous value of the alpha: " << simulator->getAlpha() << std::endl;
        std::cout 
            << "\nWhat is the value of the new alpha?"
            << std::endl;

        simulator->setAlpha(checkEntryNumber((unsigned int)1, std::numeric_limits<unsigned int>::max()));
    }
    else{
        std::cout << "Current tasks information:" << std::endl;

        // Todas as informacoes de cada uma das tarefas sao mostradas no terminal 
        const std::vector<TCB*> tasks = simulator->getTasks();
        showTasks(simulator->getTasks());

        std::cout << "Enter the ID of the task to be edited:" << std::endl;
        unsigned int idTask = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

        while(!simulator->existTaskId(idTask)){
            std::cout << "\nThe ID " << idTask << " does not match an existing ID task." << std::endl;
            std::cout << "Try again.\n" << std::endl;

            idTask = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());
        }

        clearTerminal();

        int i;
        for(i = 0; tasks[i]->getId() != idTask; i++);
        
        showTask(tasks[i]);

        std::cout << editTaskText << std::endl;
        option = checkEntryNumber((unsigned int)1, (unsigned int)7);

        clearTerminal();

        // edita o ID
        if(option == 1){
            std::cout << "Previous ID: " << tasks[i]->getId() << std::endl;
            std::cout << "What is the new ID value?\n" << std::endl;
            
            unsigned int newId = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

            while(simulator->existTaskId(newId)){
                std::cout << "The ID " << newId << " already exists." << std::endl;
                std::cout << "Try again.\n" << std::endl;

                newId = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());
            }

            // Atualiza o ID da tarefa
            simulator->updateTaskId(tasks[i]->getId(), newId);
        }
        // Edita a cor
        else if(option == 2){
            if(tasks[i]->getColor() != 0)
                std::cout << "Previous Color: " << tasks[i]->getColor() << std::endl;
            else
                std::cout << "Previous Color: " << tasks[i]->getStrColor() << std::endl;

            std::cout << "What is the new Color value?\n" << std::endl;
            std::cout << "Select or type a hexadecimal RGB color:\n\n"
                      << "Standard colors:\n"
                      << "1 -> Red\n"
                      << "2 -> Green\n"
                      << "3 -> Yellow\n"
                      << "4 -> Blue\n"
                      << "5 -> Magenta\n"
                      << "6 -> Cyan\n" << std::endl;
            
            // var pode assumir um dos dois possiveis valores (unsigned int ou string)
            std::variant<unsigned int, std::string> var = checkEntryColor((unsigned int)1, (unsigned int)6);

            // se var for unsigned int
            if(std::holds_alternative<unsigned int>(var))
                simulator->updateTaskColor(idTask, std::get<unsigned int>(var));
            // se var for string
            else if(std::holds_alternative<std::string>(var))
                simulator->updateTaskColor(idTask, std::get<unsigned int>(var));

        }
        // Edita o tempo de inicio
        else if(option == 3){
            std::cout << "Previous Entry Time: " << tasks[i]->getEntryTime() << std::endl;
            std::cout << "What is the new Entry time value?\n" << std::endl;
            
            unsigned int newEntryTime = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

            simulator->updateTaskEntryTime(tasks[i]->getId(), newEntryTime);
        }
        // Edita a duracao
        else if(option == 4){
            std::cout << "Previous Duration: " << tasks[i]->getEntryTime() << std::endl;
            std::cout << "What is the new duration value?\n" << std::endl;
            
            unsigned int newDuration = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

            simulator->updateTaskDuration(tasks[i]->getId(), newDuration);
        }
        // Edita a prioridade estatica
        else if(option == 5){
            std::cout << "Previous Priority: " << tasks[i]->getEntryTime() << std::endl;
            std::cout << "What is the new Priority value?\n" << std::endl;
            
            unsigned int newPriority = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

            simulator->updateTaskStaticPriority(tasks[i]->getId(), newPriority);
        }
        // Edita as operacoes I/O
        else if(option == 6){
            std::vector<IO_Operation> ops = tasks[i]->getIO_operations();

            // verifica se ha operacoes I/O para a tarefa
            if(!ops.size()){
                std::cout << 
                    "There isn't I/O operations for this task.\n" <<
                    "Press any key to continue.\n" <<
                std::endl;

                std::cin.get();
            }

            std::cout << 
                "What I/O operation do you want change?\n" <<
                "Press the position of the I/O operation.\n" <<
                "1 is the first operation and " << ops.size() << "is the last operation.\n" <<
            std::endl;

            unsigned int pos = checkEntryNumber((unsigned int)1, (unsigned int)ops.size());

            // escolhe o que mudar (tempo de entrada ou duracao)
            std::cout <<
                "Choose an option to edit:\n\n"
                "1 -> Operation's initial time;\n" <<
                "2 -> Operation's duration time;\n" << 
            std::endl;

            int n = checkEntryNumber((unsigned int)1, (unsigned int)2);

            // muda o tempo de entrada do I/O
            if(n == 1){
                std::cout <<
                    "Previous value: " << ops[pos - 1].getInitialTime() << "\n" <<
                    "What is the new initial time?\n" <<
                std::endl;

                if(!simulator->updateTaskIO_InitialTime(tasks[i]->getId(), ops[pos - 1].getInitialTime(), checkEntryNumber((unsigned int)0, (unsigned int)tasks[i]->getDuration()))){
                    std::cout << 
                        "This change cannot happen.\n" <<
                        "Press any key to continue.\n" <<
                    std::endl;

                    std::cin.get();
                }
            }
            // muda a duracao do I/O
            else if(n == 2){
                std::cout <<
                    "Previous value: " << ops[pos - 1].getDuration() << "\n" <<
                    "What is the new duration?\n" <<
                std::endl;

                if(!simulator->updateTaskIO_Duration(tasks[i]->getId(), ops[pos - 1].getInitialTime(), checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max()))){
                    std::cout << 
                        "This change cannot happen.\n" <<
                        "Press any key to continue.\n" <<
                    std::endl;

                    std::cin.get();
                }
            }
        }
        // altera um mutex
        else if(option == 7){
            std::vector<MutexAction> mutexesAction = tasks[i]->getMutexesAction();

            // verifica se ha mutex action para a tarefa
            if(!mutexesAction.size()){
                std::cout << 
                    "There isn't mutex actions for this task.\n" <<
                    "Press any key to continue.\n" <<
                std::endl;

                std::cin.get();
            }

            // le qual mutex id mudar
            
            std::cout << 
                "What mutex action do you want change?\n" <<
                "Enter the value of mutex ID.\n" <<
            std::endl;

            unsigned int mutexActionId = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

            // verifica se o id inserido nao existe
            while(!tasks[i]->existMutexActionId(mutexActionId)){
                std::cout << 
                    "The ID " << mutexActionId << " does not exist.\n" <<
                    "Enter again the value of mutex ID.\n" <<
                std::endl;

                mutexActionId = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());
            }

            // escolhe o que mudar (id ou tempo)
            std::cout <<
                "Choose an option to edit:\n\n"
                "1 -> Mutex ID (it'll update all mutex action with the same ID);\n" <<
                "2 -> Mutex time;\n" << 
            std::endl;

            int n = checkEntryNumber((unsigned int)1, (unsigned int)2);

            // muda o id do mutex action
            if(n == 1){
                std::cout <<
                    "Previous value: " << mutexActionId << "\n" <<
                    "What is the new ID?\n" <<
                std::endl;

                unsigned int newMutexActionId = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());

                // verifica se o id inserido existe
                while(tasks[i]->existMutexActionId(newMutexActionId)){
                    std::cout << 
                        "The ID " << newMutexActionId << " already exists.\n" <<
                        "Enter again the value of mutex ID.\n" <<
                    std::endl;

                    newMutexActionId = checkEntryNumber((unsigned int)0, std::numeric_limits<unsigned int>::max());
                }   

                simulator->updateTaskMutexesActionId(tasks[i]->getId(), mutexActionId, newMutexActionId);
            }
            // muda o tempo do mutex action
            else if(n == 2){
                std::cout << "Enter the number of the mutex action to be update.\n" << std::endl;

                // mostra os mutexes
                std::vector<MutexAction> v = tasks[i]->getMutexesAction(mutexActionId);
                size_t vTam = v.size();
                for(size_t j = 0; j < vTam; j++){
                    std::cout << j + 1 << " -> mutex ";
                    
                    // verifica o tipo
                    if(v[j].getType() == MutexAction::Type::Lock)
                        std::cout << "lock, ";
                    else if(v[j].getType() == MutexAction::Type::Unlock)
                        std::cout << "unlock, ";
                    
                    std::cout << "time: " << v[j].getTime() << std::endl;
                }

                unsigned int posMutexAction = checkEntryNumber((unsigned int)0, (unsigned int)vTam);

                std::cout <<
                    "Previous value: " << v[posMutexAction - 1].getTime() << "\n" <<
                    "What is the new time?\n" <<
                std::endl;

                unsigned int newTime = checkEntryNumber((unsigned int)0, (unsigned int)tasks[i]->getDuration());

                if(!simulator->updateTaskMutexActionTime(tasks[i]->getId(), &(v[posMutexAction - 1]), newTime)){
                    std::cout << 
                        "This change cannot happen.\n" <<
                        "Press any key to continue.\n" <<
                    std::endl;

                    std::cin.get();
                }
                else{
                    // "atualiza" o mutexAction
                    
                    tasks[i]->removeMutexAction(v[posMutexAction - 1]);

                    v[posMutexAction - 1].setTime(newTime);
                    tasks[i]->addMutexAction(v[posMutexAction - 1]);
                }
            }
        }
    }

    clearTerminal();

    std::cout << "\nDo you wish to edit more configurations? (Y or N)" << std::endl;
    std::string optionStr = checkEntryString(std::vector<std::string>{"Y", "y", "N", "n"});

    if(optionStr == "Y" || optionStr == "y")
        createEditScreen();

    clearTerminal();
}

void Menu::createAlphaScreen()
{
    clearTerminal();

    std::cout << alphaText << std::endl;

    simulator->setAlpha(checkEntryNumber((unsigned int)1, std::numeric_limits<unsigned int>::max()));

    clearTerminal();
}

// Dada um intervalo de numeros, verifica se a entrada do usuario eh valida,
// e retorna o numero escolhido
unsigned int Menu::checkEntryNumber(unsigned int firstNumber, unsigned int lastOption)
{
    int64_t num;
    std::cin >> num;

    // std::cin.fail serve para verificar se houve uma entrada invalida. Por exemplo, uma palavra
    while(std::cin.fail() || num < (int64_t)firstNumber || num > (int64_t)lastOption){
        std::cout << "\nInvalid option. Try again.\n" << std::endl;

        std::cin.clear(); // limpa flags de erro
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cin

        std::cin >> num;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cin

    return (unsigned int)num;
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
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer do cin
    }
}

std::variant<unsigned int, std::string> Menu::checkEntryColor(unsigned int firstNumber, unsigned int lastOption)
{
    while (true) {
        std::string str;
        std::cin >> str;

        // 1. Entrada numérica (formato antigo)
        if (str.size() == 1 && std::isdigit(str[0])) {
            unsigned int option = str[0] - '0';  // conversão rápida

            if (option >= firstNumber && option <= lastOption)
                return option;

            std::cout << "\nInvalid entry.\n";
        }
        // 2. Entrada hexadecimal (formato novo)
        else if (str.size() == 6 
                 && std::all_of(str.begin(), str.end(), [](unsigned char c){
                        return std::isxdigit(c); 
                    }))
        {
            return str;
        }
        // 3. Qualquer outra coisa
        else {
            std::cout << "\nInvalid entry.\n";
        }

        std::cout << "\nTry again.\n" << std::endl;

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

void Menu::showTasks(const std::vector<TCB*>& t)
{
    std::cout << "Tasks information:\n" << std::endl;
    
    size_t tam = t.size();
    for(int i = 0; i < tam; i++){
        // Verifica o tipo da cor
        if(t[i]->getColor() != 0){
            std::cout << 
                "ID: " << t[i]->getId() << "\n" <<
                "Color: " << t[i]->getColor() << "\n" <<
                "Entry time: " << t[i]->getEntryTime() << "\n" <<
                "Duration: " << t[i]->getDuration() << "\n" <<
                "Remainig time: : " << t[i]->getRemainingTime() << "\n" <<
                "Static priority: " << t[i]->getStaticPriority() << "\n" <<
                "Dynamic priority: " << t[i]->getDynamicPriority() << "\n";
        }
        else{
            std::cout << 
                "ID: " << t[i]->getId() << "\n" <<
                "Color: " << t[i]->getStrColor() << "\n" <<
                "Entry time: " << t[i]->getEntryTime() << "\n" <<
                "Duration: " << t[i]->getDuration() << "\n" <<
                "Remainig time: : " << t[i]->getRemainingTime() << "\n" <<
                "Static priority: " << t[i]->getStaticPriority() << "\n"
                "Dynamic priority: " << t[i]->getDynamicPriority() << "\n";
        }

        // Mostra as operacoes I/O
        std::vector<IO_Operation> ops = t[i]->getIO_operations();
        size_t opsTam = ops.size();
        for(size_t j = 0; j < opsTam; j++){
            std::cout <<
                "I/O initial time: " << ops[j].getInitialTime() << "\n" <<
                "I/O duration: " << ops[j].getDuration() << "\n" <<
                "I/O remainig time: " << ops[j].getRemainingTime() << "\n" <<
            std::endl;
        }

        // Mostra os mutexes action
        std::vector<MutexAction> mutexesAction = t[i]->getMutexesAction();
        size_t mutexesActionTam = mutexesAction.size();
        for(size_t j = 0; j < mutexesActionTam; j++){
            // verifica o tipo
            std::string type;
            if(mutexesAction[j].getType() == MutexAction::Type::Lock)
                type = "lock";
            else if(mutexesAction[j].getType() == MutexAction::Type::Unlock)
                type = "unlock";

            std::cout <<
                "Mutex type: " << type << "\n" <<
                "Mutex ID: " << mutexesAction[j].getId() << "\n" <<
                "Mutex time: " << mutexesAction[j].getTime() << "\n" <<
            std::endl;
        }

        // Verifica se a tarefa acabou
        if(t[i]->getEndTime() != std::numeric_limits<unsigned int>::max())
            std::cout << "End time: " << t[i]->getEndTime() << "\n";

        std::cout << std::endl;
    }
}

void Menu::showTask(const TCB *t)
{
    std::cout << "Task information:\n" << std::endl;

    // Verifica o tipo da cor
    if(t->getColor() != 0){
        std::cout << 
            "ID: " << t->getId() << "\n" <<
            "Color: " << t->getColor() << "\n" <<
            "Entry time: " << t->getEntryTime() << "\n" <<
            "Duration: " << t->getDuration() << "\n" <<
            "Remainig time: : " << t->getRemainingTime() << "\n" <<
            "Static priority: " << t->getStaticPriority() << "\n" <<
            "Dynamic priority: " << t->getDynamicPriority() << "\n";
    }
    else{
        std::cout << 
            "ID: " << t->getId() << "\n" <<
            "Color: " << t->getStrColor() << "\n" <<
            "Entry time: " << t->getEntryTime() << "\n" <<
            "Duration: " << t->getDuration() << "\n" <<
            "Remainig time: : " << t->getRemainingTime() << "\n" <<
            "Static priority: " << t->getStaticPriority() << "\n"
            "Dynamic priority: " << t->getDynamicPriority() << "\n";
    }

    // Mostra as operacoes I/O
    std::vector<IO_Operation> ops = t->getIO_operations();
    size_t opsTam = ops.size();
    for(size_t j = 0; j < opsTam; j++){
        std::cout <<
            "I/O initial time: " << ops[j].getInitialTime() << "\n" <<
            "I/O duration: " << ops[j].getDuration() << "\n" <<
            "I/O remainig time: " << ops[j].getRemainingTime() << "\n";
    }

    // Mostra os mutexes action
    std::vector<MutexAction> mutexesAction = t->getMutexesAction();
    size_t mutexesActionTam = mutexesAction.size();
    for(size_t j = 0; j < mutexesActionTam; j++){
        // verifica o tipo
        std::string type;
        if(mutexesAction[j].getType() == MutexAction::Type::Lock)
            type = "lock";
        else if(mutexesAction[j].getType() == MutexAction::Type::Unlock)
            type = "unlock";

        std::cout <<
            "Mutex type: " << type << "\n" <<
            "Mutex ID: " << mutexesAction[j].getId() << "\n" <<
            "Mutex time: " << mutexesAction[j].getTime() << "\n";
    }

    // Verifica se a tarefa acabou
    if(t->getEndTime() != std::numeric_limits<unsigned int>::max())
        std::cout << "End time: " << t->getEndTime() << "\n";

    std::cout << std::endl;
}
