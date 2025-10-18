#include "GanttChartGenerator.h"

GanttChartGenerator::GanttChartGenerator()
{
}

GanttChartGenerator::~GanttChartGenerator()
{
}

void GanttChartGenerator::createAxis(size_t numTasks, std::vector<unsigned int> idTasks, unsigned int sumDurationTasks)
{
    // Calcula o tamanho do eixo Y
    // 13 corresponde a altura em pixels do retangulo
    // 2 serve para separar cada retangulo
    // 1 serve para separar o ultimo retangulo do eixo X
    size_t tamY = numTasks * 13 * 2 + 1;

    // Calcula o eixo X em base de Y
    size_t tamX = tamY * 1.5;
    if(tamX > 1920)
        tamX = 1920;

    // Cria os eixos com os nomes deles
    axis << "\n" <<
        R"(
        <text x="15" y="30" font-size="20" fill="black">ID tasks</text>
        <text x="36.5" y="78" font-size="40" fill="black">^</text>
        <rect x="45" y="60" width="2" height=")" << tamY << R"(" fill="black"></rect>
        <rect x="45" y=")" << tamY + 60 << R"(" width=")" << tamX << R"(" height="2" fill="black"></rect>
        <text x=")" << tamX + 45 - 5 << R"(" y=")" << tamY + 60 + 11<< R"(" font-size="30" fill="black">></text>
        <text x=")" << tamX + 45 - 5 << R"(" y=")" << tamY + 60 + 7.5<< R"(" font-size="20" fill="black">time (ticks)</text>)";

    // Preenche o eixo Y com os ID's das tarefas
    std::sort(idTasks.begin(), idTasks.end());
    for(size_t i = 0; i < numTasks; i++){
        // tamY + 60 para saber a posicao do canto superior esquerdo do eixo Y
        // -13 para "desgrudar" o valor do ID do eixo X 
        axis << "\n" << R"(<text x="30" y=")" << tamY + 60 - 13 << R"(" font-size="10" fill="black">)" << idTasks[i] << R"(</text>)";
    }

    // Preenche o ultimo tick execudo no escalonador
    axis << "\n" <<
        R"(
        <text x=")" << tamX + 45 << R"(" y=")" << tamY + 60 + 17.5 << R"(" font-size="10" fill="black">)" << sumDurationTasks << R"(</text>)";
}

void GanttChartGenerator::addRectTask(TCB &task)
{
}
