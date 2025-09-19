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

const std::string Menu::confirmationText = R"(
    Confirme as configuracoes:
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
    std::cout << inicialText << std::endl;
    std::cin.get();

    clearTerminal();

    // Controla as diferentes entradas do usuario
    int option = -1;
    while(option < 1 || option > 2){
        std::cout << arquiveText << std::endl;
        std::cin >> option;
    }

    if(option == 1){
        
    }
}

void Menu::clearTerminal(){
#ifdef _WIN32
	system("cls");  // Windows
#else
	system("clear");  // Linux/macOS
#endif
}