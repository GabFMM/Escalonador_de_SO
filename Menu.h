#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <variant>
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
    static const std::string editText;
    static const std::string editTaskText;

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
    void createEditScreen();

    // metodos auxiliares
    unsigned int checkEntryNumber(unsigned int firstNumber, unsigned int lastOption);
    std::string checkEntryString(std::vector<std::string> targets);
    std::variant<unsigned int, std::string> checkEntryColor(unsigned int firstNumber, unsigned int lastOption);
    void clearTerminal();
    void setSimulator(Simulator* s);
    void showTasks();
};