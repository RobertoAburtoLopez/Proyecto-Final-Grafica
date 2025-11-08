#pragma once
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"
#include <vector>
#include <string>

// ---------------------------------------------------------------------- === Estructuras de Datos ===
struct Animal 
{
    std::string nombre;
    std::string ruta;
    Model modelo;
};

class ResourceManager {
public:
    // ------------------------------------------------------------------ === Constructor de Clase ===

    ResourceManager();

    // ------------------------------------------------------------------ === Skybox ===

    Skybox skybox;

    // ------------------------------------------------------------------ === Texturas ===

    Texture plainTexture;
    Texture pastoTexture;
    Texture croquisTexture;
    Texture LetreroTexture;

    // ------------------------------------------------------------------ === Arena Central ===

    Model ArenaCentral;

    Model Guardianes_M;

    Model Entrada_M;
    Model EntradaPuertaDerecha_M;
    Model EntradaPuertaIzquierda_M;
    Model Cartel_M;
    
    std::vector<Model> DecoracionCentral;

    // ------------------------------------------------------------------ === Piramides ===
    
    Model PiramideChicken;
    Model PiramideChilly;
    Model PiramideRikoche;
    Model PiramideTotoro;

    // ------------------------------------------------------------------ === Animales Zoológico ===

    std::vector<Animal> Zoologico;

    // ------------------------------------------------------------------ === Personajes Principales ===

    Model ChickenLittle;
    Model Chilly_Willy;
    Model Rikoche;
    Model Totoro, Totoro_mediano, Totoro_chiquito;

    // ------------------------------------------------------------------ === Universo Chiquito Chicken ===

    std::vector<Model> EdificiosLittle;

    Model CalleChicken;
    Model CieloHexagonal;
    Model Hexagono;

    Model MariposaAzul, MariposaAzul_AlaDerecha, MariposaAzul_AlaIzquierda;

    std::vector<Model> Arboles;

    Model AbbyPatosa, Runt, PezOutWater, Kirby, BuckGallo, Melvin, Tina, Ace, RobotAlien, TheDog;

    // ------------------------------------------------------------------ === Universo Chilly Willy ===

    Model Lago;
    Model Arbol_Hielo;
    Model Iglu;
    Model Anuncio;
    Model Animalitos;

    // ------------------------------------------------------------------ === Universo Rikoche ===
     
     
    
    // ------------------------------------------------------------------ === Universo Totoro ===
    Model conejo_universo_totoro;
    Model buho_universo_totoro;
    Model tortuga_universo_totoro;
    Model gato_universo_totoro;

};