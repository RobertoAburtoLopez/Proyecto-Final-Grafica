#pragma once
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"
#include <vector>
#include <string>

class ResourceManager {
public:
    // Constructor
    ResourceManager();

    // Skybox
    Skybox skybox;

    // Texturas
    Texture brickTexture, dirtTexture, plainTexture, pisoTexture;

    // Modelos Principales
    Model Chicken_Little;
    Model Chilly_Willy;
    Model Rikoche;
    Model Totoro, Totoro_mediano, Totoro_chiquito;

    // Modelos del Zoo
    Model animales;

    // Modelos de Estructuras
    Model Cuadrilatero;
    Model PiramideChicken;
    Model PiramideChilly;
    Model PiramideRikoche;
    Model PiramideTotoro;
    
};
