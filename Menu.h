#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "TCB.h"

class Simulator;

class Menu{
private:
    Simulator* simulator;
    static const std::string inicialText;
    static const std::string arquiveText;
    static const std::string algorithmText;
    static const std::string taskText;
    static const std::string chosenModeText;

public:
    Menu();
    ~Menu();

    // metodo principal
    void execute();
    
    // metodos para criar partes do menu
    void createInicialScreen();
    void createArquiveScreen();
    void createConfirmationScreen(std::vector<TCB> tasks);
    void createChosenModeScreen();
    void createAlgorithmScreen();
    void createTaskScreen();

    // metodos auxiliares
    int checkEntryNumber(int firstOption, int lastOption);
    std::string checkEntryString(std::vector<std::string> targets);
    void clearTerminal();
};