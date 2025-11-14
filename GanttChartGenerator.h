#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <string>
#include <iostream>

#include "TCB.h"

class GanttChartGenerator{
private:
    std::ostringstream buffer; // o buffer nao inclui o cabecalho padrao do svg

    // first eh a coordenada X, second eh a coordenada Y
    std::pair<unsigned int, unsigned int> posAxisY; 
    std::pair<unsigned int, unsigned int> posAxisX; 
    unsigned int tamAxisY;
    unsigned int tamAxisX; 

    // tabela hash, pois haverá mais buscas
    std::unordered_map<unsigned int, unsigned int> idPosYTasks; 

    // relacao ticks por pixel
    // usado no eixo X
    double tpp;

public:
    GanttChartGenerator();
    ~GanttChartGenerator();

    void createAxis(size_t numTasks, std::vector<unsigned int> idTasks, unsigned int sumDurationTasks, unsigned int maxEntryTime);

    void addRectTask(const unsigned int& idTask, const unsigned int& colorTask, unsigned int timeNow, unsigned int timeLastInterrupt);
    void addRectTask(const unsigned int& idTask, const std::string& colorTask, unsigned int timeNow, unsigned int timeLastInterrupt);

    std::string toStrColor(const unsigned int& color);

    void generateImage();
};