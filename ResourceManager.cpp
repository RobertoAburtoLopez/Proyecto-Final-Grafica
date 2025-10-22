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
    brickTexture = Texture("Texturas/ladrillo_rojo.png");   brickTexture.LoadTextureA();
    dirtTexture = Texture("Texturas/suelo_seco.png");     dirtTexture.LoadTextureA();
    plainTexture = Texture("Texturas/plain.png");   plainTexture.LoadTextureA();
    pisoTexture = Texture("Texturas/pasto.png");    pisoTexture.LoadTextureA();

    // Modelos Principales
    Chicken_Little.LoadModel("Modelos/Chicken Little/Chicken Little.obj");
    Chilly_Willy.LoadModel("Modelos/Chilly Willy/Chilly Willy.obj");
    Rikoche.LoadModel("Modelos/Rikoche/Rikoche.obj");
    Totoro.LoadModel("Modelos/Totoro/Totoro.obj");
    Totoro_mediano.LoadModel("Modelos/Totoro/Totoro_mediano.obj");
    Totoro_chiquito.LoadModel("Modelos/Totoro/Totoro_chiquito.obj");

    // Modelos del Zoo
    animales.LoadModel("Modelos/Zoo/.obj");

    // Modelos de Estructuras
    Cuadrilatero.LoadModel("Modelos/Estructuras/Cuadrilatero.obj");
    PiramideChicken.LoadModel("Modelos/Prehispanicos/Piramide de la Venta/Piramide de la Venta.obj");
    PiramideChilly.LoadModel("Modelos/Estructuras/Piramide Chilly Willy.obj");
    PiramideRikoche.LoadModel("Modelos/Estructuras/Piramide Rikoche.obj");
    PiramideTotoro.LoadModel("Modelos/Estructuras/Piramide Totoro.obj");
}