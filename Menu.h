#pragma once

#include <iostream>
#include <string>

class Simulator;

class Menu{
private:
    Simulator* simulator;
    static const std::string inicialText;
    static const std::string arquiveText;
    static const std::string confirmationText;
    static const std::string algorithmText;
    static const std::string taskText;
    static const std::string chosenModeText;

public:
    Menu();
    ~Menu();

    void execute();
    void clearTerminal();
};