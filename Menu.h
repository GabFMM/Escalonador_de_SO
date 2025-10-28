#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <limits>
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
    static const std::string quantumText;

    // New tasks
    static const std::string colorText;

public:
    Menu();
    ~Menu();

    // metodo principal
    void execute();
    
    // metodos para criar partes do menu
    void createInicialScreen();
    void createArquiveScreen();
    void createConfirmationScreen(const std::vector<TCB*>& tasks);
    void createChosenModeScreen();
    void createAlgorithmScreen();
    void createTaskScreen();
    void createQuantumScreen();

    // metodos auxiliares
    unsigned int checkEntryNumber(unsigned int firstNumber, unsigned int lastOption);
    std::string checkEntryString(std::vector<std::string> targets); // TO-DO: erro de laco infinito ao executar
    void clearTerminal();
    void setSimulator(Simulator* s);
};