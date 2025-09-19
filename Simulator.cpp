#include "Simulator.h"
#include "Menu.h"

Simulator::Simulator() :
menu(nullptr),
scheduler(nullptr),
imageGenerator(nullptr),
extraInfo(nullptr){

}

Simulator::~Simulator()
{
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

    if(extraInfo != nullptr){
        delete extraInfo;
        extraInfo = nullptr;
    }
}

void Simulator::start()
{
    menu = new Menu();
    menu->execute();
}
