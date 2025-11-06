#pragma once
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"
#include <vector>
#include <string>

struct Animal 
{
    std::string nombre;
    std::string ruta;
    Model modelo;
};

class ResourceManager {
public:
    // Constructor
    ResourceManager();

    // Skybox
    Skybox skybox;

    // Texturas
    Texture plainTexture;
    Texture pastoTexture;
    Texture croquisTexture;
    Texture LetreroTexture;

    // Modelos para las piramides
    Model ArenaCentral;
    Model PiramideChicken;
    Model PiramideChilly;
    Model PiramideRikoche;
    Model PiramideTotoro;

    // Lista de modelos para el Zool�gico
    std::vector<Animal> Zoologico;

    // Modelos Principales
    Model Chicken_Little;
    Model Chilly_Willy;
    Model Rikoche;
    Model Totoro, Totoro_mediano, Totoro_chiquito;

    // Modelos de cada universo
    
    // Estructuras
    Model Entrada_M;
    Model EntradaPuertaDerecha_M;
    Model EntradaPuertaIzquierda_M;
    Model Cartel_M;
};
