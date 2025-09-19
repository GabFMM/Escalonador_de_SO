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

    }

    clearTerminal();
}

void Menu::createConfirmationScreen(std::vector<TCB> tasks)
{
    clearTerminal();

    std::cout << "Confirme as configuracoes:\n" << std::endl;
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
    checkEntryNumber(1, 2);

    clearTerminal();
}

// Dada uma sequencia de opcoes, verifica se a entrada do usuario eh valida,
// e retorna a opcao escolhida
int Menu::checkEntryNumber(int firstOption, int lastOption)
{
    int option = -1;
    while(option < firstOption || option > lastOption){
        std::cout << "\nOpcao invalida. Tente novamente.\n" << std::endl;
        std::cin >> option;
    }

    return option;
}

void Menu::clearTerminal()
{
#ifdef _WIN32
	system("cls");  // Windows
#else
	system("clear");  // Linux/macOS
#endif
}