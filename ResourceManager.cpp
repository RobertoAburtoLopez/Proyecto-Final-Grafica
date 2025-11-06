#include "ResourceManager.h"

ResourceManager::ResourceManager() {

    // Skybox
    std::vector<std::string> blueSky;
    blueSky.push_back("Texturas/Skybox/blueSky/derecha.png");		// right (voltear verticalmente)
    blueSky.push_back("Texturas/Skybox/blueSky/izquierda.png");		// left
    blueSky.push_back("Texturas/Skybox/blueSky/abajo.png");			// bottom
    blueSky.push_back("Texturas/Skybox/blueSky/arriba.png");		// up (girar 90° en sentido antihorario)
    blueSky.push_back("Texturas/Skybox/blueSky/atras.png");			// back
    blueSky.push_back("Texturas/Skybox/blueSky/enfrente.png");		// front
    skybox = Skybox(blueSky);

    // Texturas
    plainTexture = Texture("Texturas/plain.png");   plainTexture.LoadTextureA();
    pastoTexture = Texture("Texturas/Pasto.png");   pastoTexture.LoadTextureA();
    croquisTexture = Texture("Texturas/Croquis.jpg");  croquisTexture.LoadTextureA();

    // Modelos de las piramides
    //ArenaCentral.LoadModel("Modelos/Prehispanico/Arena Central.obj");
    /*PiramideChilly.LoadModel("Modelos/Piramides/Piramide de Quetzalcoatl.obj");
    PiramideRikoche.LoadModel("Modelos/Piramides/Templo de Lucha.obj");
    PiramideTotoro.LoadModel("Modelos/Piramides/Templo de Kukulcan.obj");*/

    // Modelos de los animales del Zoológico
    /*Zoologico = 
    {
        {"Zorro",        "Modelos/Zoologico/Zorro.obj"},
        {"Tigre",        "Modelos/Zoologico/Tigre.obj"},
        {"Rinoceronte",  "Modelos/Zoologico/Rinoceronte.obj"},
        {"Pantera",      "Modelos/Zoologico/Pantera.obj"},
        {"Panda Rojo",   "Modelos/Zoologico/Panda Rojo.obj"},
        {"Orangutan",    "Modelos/Zoologico/Orangutan.obj"},
        {"Mapache",      "Modelos/Zoologico/Mapache.obj"}
    };
    for (auto& animal : Zoologico)
        animal.modelo.LoadModel(animal.ruta);*/

    // Personajes principales
    ChickenLittle.LoadModel("Modelos/Chicken Little/Chicken Little.obj");

    /*Chilly_Willy.LoadModel("Modelos/Chilly Willy/Chilly Willy.obj");
    Rikoche.LoadModel("Modelos/Rikoche/Rikoche.obj");
    Totoro.LoadModel("Modelos/Totoro/Totoro.obj");
    Totoro_mediano.LoadModel("Modelos/Totoro/Totoro_mediano.obj");
    Totoro_chiquito.LoadModel("Modelos/Totoro/Totoro_chiquito.obj");*/

    // Universo Chiquito Chicken
    PiramideChicken.LoadModel("Modelos/Piramides/Piramide de Chicken.obj");

    EdificiosLittle[0].LoadModel("Modelos/Chicken Little/Chicken Casa.obj");
    EdificiosLittle[1].LoadModel("Modelos/Chicken Little/Ayuntamiento.obj");
    EdificiosLittle[2].LoadModel("Modelos/Chicken Little/Estacion de Bomberos.obj");
    EdificiosLittle[3].LoadModel("Modelos/Chicken Little/Torre de Agua.obj");
    EdificiosLittle[4].LoadModel("Modelos/Chicken Little/Edificio Naranja.obj");
    EdificiosLittle[5].LoadModel("Modelos/Chicken Little/Edificio Tienda.obj");
    EdificiosLittle[6].LoadModel("Modelos/Chicken Little/Edificio Oaks.obj");

    CalleChicken.LoadModel("Modelos/Chicken Little/Calle Chicken.obj");
    CieloHexagonal.LoadModel("Modelos/Chicken Little/Cielo Hexagonal.obj");

    Arboles.push_back(Model("Modelos/Chicken Little/Arbol 1.obj"));
    Arboles.push_back(Model("Modelos/Chicken Little/Arbol 2.obj"));

    AbbyPatosa.LoadModel(   "Modelos/Chicken Little/Abby Patosa.obj");
    Runt.LoadModel(         "Modelos/Chicken Little/Runt.obj");
    PezOutWater.LoadModel(  "Modelos/Chicken Little/PezOutWater.obj");
    Kirby.LoadModel(        "Modelos/Chicken Little/Kirby.obj");
    BuckGallo.LoadModel(    "Modelos/Chicken Little/Buck Gallo.obj");
    Melvin.LoadModel(       "Modelos/Chicken Little/Melvin.obj");
    Tina.LoadModel(         "Modelos/Chicken Little/Tina.obj");
    Ace.LoadModel(          "Modelos/Chicken Little/Ace.obj");
    RobotAlien.LoadModel(   "Modelos/Chicken Little/Robot Alien.obj");
    TheDog.LoadModel(       "Modelos/Chicken Little/The Dog.obj");
}