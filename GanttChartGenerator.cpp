#include "GanttChartGenerator.h"

GanttChartGenerator::GanttChartGenerator()
{
    idPosYTasks.max_load_factor(0.7f); // Reduz colisões, mas aumenta o custo de memória
}

GanttChartGenerator::~GanttChartGenerator()
{
    idPosYTasks.clear();
}

// sumDurationTasks deve estar em ticks
void GanttChartGenerator::createAxis(size_t numTasks, std::vector<unsigned int> idTasks, unsigned int sumDurationTasks, unsigned int maxEntryTime)
{
    // Calcula o tamanho do eixo Y
    // 13 corresponde a altura em pixels do retangulo
    // 2 serve para separar cada retangulo
    // +13 serve para separar o ultimo retangulo do eixo X
    unsigned int tamY = numTasks * 13 * 2 + 13;
    tamAxisY = tamY;
    posAxisY.first = 45;
    posAxisY.second = 60;

    // Calcula o eixo X em base de Y
    unsigned int tamX = tamY * 1.5;
    if(tamX > 1920)
        tamX = 1920;
    tamAxisX = tamX;
    posAxisX.first = 45;
    posAxisX.second = posAxisY.second + tamAxisY;

    // Define a relacao ticks por pixel
    tpp = (double)tamAxisX / ((double)sumDurationTasks + (double)maxEntryTime);

    // Cria os eixos com os nomes deles
    buffer << "\n" << 
        R"(
        <text x="15" y="30" font-size="20" fill="black">ID tasks</text>
        <text x="36.5" y="78" font-size="40" fill="black">^</text>
        <rect x="45" y="60" width="2" height=")" << tamY << R"(" fill="black"></rect>
        <rect x="45" y=")" << tamY + 60 << R"(" width=")" << tamX << R"(" height="2" fill="black"></rect>
        <text x=")" << tamX + 45 - 5 << R"(" y=")" << tamY + 60 + 11<< R"(" font-size="30" fill="black">></text>
        <text x=")" << tamX + 60 << R"(" y=")" << tamY + 60 + 7.5<< R"(" font-size="20" fill="black">time (ticks)</text>)";

    // Preenche o eixo Y com os ID's das tarefas
    idPosYTasks.reserve(numTasks); // limita o tamanho da tabela hash
    std::sort(idTasks.begin(), idTasks.end());
    for(size_t i = 0; i < numTasks; i++){
        // tamY + 60 para saber a posicao do canto inferior esquerdo do eixo Y
        // -13 para "desgrudar" o valor do ID do eixo X 
        unsigned int posId = tamY + 60 - 13 * (i+1);

        buffer << "\n" << R"(<text x="30" y=")" << posId << R"(" font-size="10" fill="black">)" << idTasks[i] << R"(</text>)";

        // guarda a relacao id e posicao
        idPosYTasks.insert({idTasks[i], posId});
    }

    // Preenche o ultimo tick execudo no escalonador
    buffer << "\n" <<
        R"(
        <text x=")" << tamX + 45 << R"(" y=")" << tamY + 60 + 17.5 << R"(" font-size="10" fill="black">)" << sumDurationTasks + maxEntryTime << R"(</text>)";
}

// time in ticks
// timeLastInterrupt se refere a interrupcao do escalonador, nao da tarefa
void GanttChartGenerator::addRectTask(const unsigned int& idTask, const unsigned int& colorTask, unsigned int timeNow, unsigned int timeLastInterrupt)
{
    unsigned int posYId = idPosYTasks[idTask];

    // Cria o retangulo de execucao da tarefa
    buffer << "\n" <<
        R"(
        <rect x=")" << std::fixed << std::setprecision(2) << timeLastInterrupt * tpp + posAxisX.first + 2 << R"(" y=")" << posYId - 10 << R"(" width=")" << std::fixed << std::setprecision(2) << (timeNow * tpp) - (timeLastInterrupt * tpp) - 2 << R"(" height=")" << 13 << R"(" fill=")" << toStrColor(colorTask) << R"("></rect>)";

    // Coloca os indices de tempos no eixo X
    buffer << "\n" << // indice de inicio de tarefa
        R"(
        <text x=")" << timeLastInterrupt * tpp + posAxisX.first << R"(" y=")" << posAxisX.second + 17.5 << R"(" font-size="10" fill="black">)" << timeLastInterrupt << R"(</text>)";

    buffer << "\n" << // indice de fim de tarefa
        R"(
        <text x=")" << timeNow * tpp + posAxisX.first << R"(" y=")" << posAxisX.second + 17.5 << R"(" font-size="10" fill="black">)" << timeNow << R"(</text>)";
}

std::string GanttChartGenerator::toStrColor(const unsigned int &color)
{
    std::string colors[6];
    colors[0] = "red";
    colors[1] = "green";
    colors[2] = "yellow";
    colors[3] = "blue";
    colors[4] = "magenta";
    colors[5] = "cyan";

    return colors[color - 1];
}

void GanttChartGenerator::generateImage()
{
    std::ofstream arquive("image.svg");

    if (!arquive.is_open()) {
        std::cerr << "Erro ao abrir o arquivo SVG.\n";
        return;
    }

    // cabecalho
    arquive << R"(<?xml version="1.0" encoding="UTF-8"?>)" << '\n';
    arquive << R"(<svg xmlns="http://www.w3.org/2000/svg" width=")" << tamAxisX + posAxisX.first + 120 << R"(" height=")" << tamAxisY + posAxisY.second + 50 << R"(">)" << '\n';

    // miolo
    arquive << buffer.str();

    // fim
    arquive << "</svg>\n";

    arquive.close();
}
