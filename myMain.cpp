
#define STB_IMAGE_IMPLEMENTATION	//	Para cargar imágenes
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdio.h>					//			...
#include <string.h>					//			...
#include <cmath>					//			...
#include <vector>					//			...
#include <math.h>					//			...
#include <glew.h>					//			...
#include <glfw3.h>					//			...
#include <glm.hpp>					//			...
#include <gtc/matrix_transform.hpp>	//			...
#include <gtc/type_ptr.hpp>			//			...
#include "Window.h"					//	Window --
#include "Mesh.h"					//	Mesh --
#include "Shader_light.h"			//	Shader --
#include "Camera.h"					//	Camera --
#include "Texture.h"				//	Texture --
#include "Sphere.h"					//			...
#include "Model.h"					//	Model
#include "Skybox.h"					//	Skybox
#include "CommonValues.h"			//			...
#include "DirectionalLight.h"		//	Iluminación --
#include "PointLight.h"				//			...
#include "SpotLight.h"				//			...
#include "Material.h"				//	Material --
#include "ResourceManager.h"		//	Recursos --
#include "KeyframeAnimation.h"		//	Animacion --
#include <fstream>					//			...
#include <iostream>					//			...
#include <sstream>					//			...
#include <chrono>					//			...
//#include<assimp/Importer.hpp>		// Para probar el importer

// ------------------------------------------------------------------ Audio
ma_engine engine;
ma_sound soundQ[5];
ma_sound ambiente;
int currentQuadrant = -1;
ma_sound soundZona;
bool zonaActiva = false;
bool sonidoCargado = false;

// ------------------------------------------------------------------ Skybox
float cieloTime = 0.0f, cielo_max = 2000.0f;
int cieloActual = 0;


// ------------------------------------------------------------------ Ventana

Window mainWindow;

std::vector<Mesh*> meshList;		// Vector de Meshs
std::vector<Shader> shaderList;		// Vector de Shaders

// ------------------------------------------------------------------ Camara

Camera Pov3erCamera;
Camera PovAereaCamera;
Camera PovSitesCamera;

Camera* cameraActual = &Pov3erCamera;
bool cPressed = false;



glm::vec3 offsetChicken_camera = glm::vec3(0.0f, 70.0f, 170.0f);
static double limitFPS = 1.0 / 60.0;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// ------------------------------------------------------------------ Shader

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

// ------------------------------------------------------------------ Material

Material Material_brillante;
Material Material_opaco;
Material Material_Chicken_piel, Material_Chicken_piernas;


// ------------------------------------------------------------------ Iluminacion

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

DirectionalLight mainLight[5];					// Luz Direccional (Sol)
PointLight PL_0[MAX_POINT_LIGHTS];			// Arreglos de luces PointLight
SpotLight PL_1[MAX_SPOT_LIGHTS];			//.
SpotLight SP_RGB[MAX_SPOT_LIGHTS];			// Arreglos de luces SpotLight
SpotLight SP_lamparas[MAX_SPOT_LIGHTS];		//.

glm::vec3 posLamp1 = glm::vec3(0);
glm::vec3 posLamp2 = glm::vec3(0);
glm::vec3 posLamp3 = glm::vec3(0);
glm::vec3 posLamp4 = glm::vec3(0);

// ------------------------------------------------------------------ Animacion

// CAMARAS DE ESQUINA
int currentCornerCamera = 0;              // Índice de la cámara actual (0-3)
float cornerCameraTimer = 0.0f;           // Temporizador para cambio de cámara
float cornerCameraInterval = 150.0f;       // Intervalo de 15 segundos entre cámaras

// Animacion Mariposa - aleteo
float speedAleteo = 15.0f;
float angulo = 0.0f;
float tiempo = 0.0f;
float deltaTiempo = 0.005f;
glm::vec3 posMariposaAlas = glm::vec3(0.0f, 0.0f, 0.0f);

// Animacion Totoro - caminata
float movTotoro = 0.0f, movBrazosTotoro = 0.0f, movPiernasTotoro = 0.0f;
float dirTotoro1 = 0.01f;
float dirTotoro2 = 0.01f;
float dirTotoro3 = 0.01f;

// Animacion Chicken Little - caminata
float movChicken = 0.0f, movBrazosChicken = 0.0f, movPiernaChicken = 0.0f;
float dirChicken1 = 0.01f;
float dirChicken2 = 0.05f;
float dirChicken3 = 0.01f;

glm::vec3 posChicken = glm::vec3(0.0f, 0.0f, 1000.0f);
glm::vec3 dirChicken = glm::vec3(0);
float speedChicken = 0.35f;
float giroChicken = 0.0f;
float speedGiroChicken = 0.45f;


// Animacion cielo hexagonal
float posHexagonos = 0.0f;
float speedHexagonos = -0.25f;

KeyframeAnimation anim;

// letrero 
float toffsetLetrero = 0.0f;
float toffsetLetrerov = 0.0f;
float toffsetLetrerou = 0.0f;

// PUERTAS 
float rotacionPuertaDerecha = 0.0f;      // Rotación de puerta derecha (0 a 90 grados)
float deslizamientoPuertaIzq = 0.0f;     // Deslizamiento puerta izquierda (0 a 20 unidades)
bool puertasAbiertas = false;             // Estado de las puertas
bool animandoPuertas = false;             // Bandera para controlar animación



// ------------------------------------------------------------------ Funciones

// Función - Cálculo de Normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

// Función - Creamos las primitivas
void CreatePrimitives()
{
	// --------------------------------------------------------- Suelo
	GLfloat vertices_Suelo[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-10.0f, 0.0f, -10.0f,	0.0f, 1.0f,		0.0f, -1.0f, 0.0f, // indice 0 - arriba a la izquierda
			10.0f, 0.0f, -10.0f,	1.0f, 1.0f,		0.0f, -1.0f, 0.0f, // indice 1 - arriba a la derecha
			-10.0f, 0.0f, 10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f, // indice 2 - abajo a la izquierda
			10.0f, 0.0f, 10.0f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f  // indice 3 - abajo a la derecha
	};
	unsigned int indices_Suelo[] = {
		0, 2, 1,
		1, 2, 3
	};

	Mesh* Suelo = new Mesh();
	Suelo->CreateMesh(vertices_Suelo, indices_Suelo, 32, 6);
	meshList.push_back(Suelo);

	// --------------------------------------------------------- Vegetación
	GLfloat vertices_Vegetacion[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

			0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	unsigned int indices_Vegetacion[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	Mesh* Vegetacion = new Mesh();
	Vegetacion->CreateMesh(vertices_Vegetacion, indices_Vegetacion, 64, 12);
	meshList.push_back(Vegetacion);

	calcAverageNormals(indices_Vegetacion, 12, vertices_Vegetacion, 64, 8, 5);

	// --------------------------------------------------------- Plano para letrero (cuadrado simple)
	GLfloat vertices_Letrero[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f
	};
	unsigned int indices_Letrero[] = {
		0, 1, 2,
		0, 2, 3
	};

	Mesh* Letrero = new Mesh();
	Letrero->CreateMesh(vertices_Letrero, indices_Letrero, 32, 6);
	meshList.push_back(Letrero);
}

// Función - Creamos los shaders
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// Función - Creamos la iluminación
void SetLights()
{
	// Amanecer
	mainLight[0] = DirectionalLight(
		0.65f, 0.78f, 1.0f,   
		0.35f, 0.15f,       
		-0.2f, -1.0f, -0.1f
	);

	// Mañana
	mainLight[1] = DirectionalLight(
		1.0f, 0.82f, 0.90f,  
		0.45f, 0.25f,
		-0.3f, -1.0f, -0.2f
	);

	// Atardecer
	mainLight[2] = DirectionalLight(
		1.0f, 0.72f, 0.40f, 
		0.30f, 0.35f,
		-0.1f, -1.0f, -0.1f
	);

	// Anochecer
	mainLight[3] = DirectionalLight(
		0.72f, 0.40f, 0.72f,
		0.30f, 0.35f,
		0.0f, -1.0f, 0.0f
	);

	// Estrellado
	mainLight[4] = DirectionalLight(
		0.10f, 0.14f, 0.42f, 
		0.20f, 0.35f,
		0.0f, -1.0f, 0.0f
	);


// Verde
	SP_lamparas[0] = SpotLight(0.0f, 1.0f, 0.0f,  // Color
		1.0f, 5.0f,                               // Intensidad ambiental | Intensidad difusa
		20.0f, 15.0f, 0.0f,                        // Posición
		0.0f, -1.0f, 0.0f,                         // Dirección
		0.2f, 0.009f, 0.0f,                          // Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                     // Ángulo edge
	spotLightCount++;

	// Azul
	SP_lamparas[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 5.0f,
		10.0f, 15.0f, 10.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.009f, 0.0f,
		20.0f);
	spotLightCount++;

	// Roja
	SP_lamparas[2] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 5.0f,
		-10.0f, 15.0f, 10.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.009f, 0.01f,
		20.0f);
	spotLightCount++;

	// Blanca
	SP_lamparas[3] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 5.0f,
		-20.0f, 15.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.2f, 0.009f, 0.01f,
		20.0f);
	spotLightCount++;

	// Arreglo 2 para cambio de luces
	// Blanca
	SP_RGB[0] = SP_lamparas[3];
	// Verde
	SP_RGB[1] = SP_lamparas[0];
	// Azul
	SP_RGB[2] = SP_lamparas[1];
	// Roja
	SP_RGB[3] = SP_lamparas[2];
}

// Función - Renderizar modelo
void RenderModel(glm::mat4 model, GLuint uniformModel, glm::vec3 origen, glm::vec3 pos, Model& modelRender)
{	
	model = glm::translate(model, origen + pos);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	modelRender.RenderModel();
}

// Función - Renderizar modelo con escala
void RenderModelScale(glm::mat4 model, GLuint uniformModel, glm::vec3 origen, glm::vec3 pos, float scal, Model& modelRender)
{
	model = glm::translate(model, origen + pos);
	model = glm::scale(model, glm::vec3(scal));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	modelRender.RenderModel();
}

// Función - Renderizar modelo con rotacion
void RenderModelRotate(glm::mat4 model, GLuint uniformModel, glm::vec3 origen, glm::vec3 pos, float rot, glm::vec3 direccion, Model& modelRender)
{
	model = glm::translate(model, origen + pos);
	model = glm::rotate(model, glm::radians(rot), glm::vec3(direccion));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	modelRender.RenderModel();
}

// Función - Renderizar mesh
void RenderMeshWithTexture(Mesh* mesh, glm::vec3 color, glm::vec3 position, glm::vec3 scale,
	GLuint uniformModel, GLuint uniformColor, GLuint uniformSpecularIntensity, GLuint uniformShininess,
	Material* material = nullptr, Texture* texture = nullptr)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);                    // Reiniciar la matriz
	model = glm::translate(model, position);	// Posición
	model = glm::scale(model, scale);			// Escala

	if (texture != nullptr)
		texture->UseTexture();

	if (material != nullptr)
		material->UseMaterial(uniformSpecularIntensity, uniformShininess);

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));

	mesh->RenderMesh();
}

// Función - Animaciones Universo Totoro
void animationsTotoro()
{
	// Caminata de Totoro
	movTotoro += dirTotoro1;
	movBrazosTotoro += dirTotoro2;
	movPiernasTotoro += dirTotoro3;

	if (movTotoro > 50.0f) dirTotoro1 = -0.25f;
	else if (movTotoro < -50.0f) dirTotoro1 = 0.25f;
	if (movBrazosTotoro > 10.0f) dirTotoro2 = -0.5f;
	else if (movBrazosTotoro < -10.0f) dirTotoro2 = 0.5f;
	if (movPiernasTotoro > 20.0f) dirTotoro3 = -0.7f;
	else if (movPiernasTotoro < -20.0f) dirTotoro3 = 0.7f;
}

// Función - Animaciones Universo Chicken Little
void animationsChickenLittle()
{
	movChicken += dirChicken1;
	movBrazosChicken += dirChicken2;
	movPiernaChicken += dirChicken3;

	// Movimiento de Manos y Pies (ciclico)
	if (movChicken > 50.0f) dirChicken1 = -0.25f;
	else if (movChicken < -50.0f) dirChicken1 = 0.25f;
	if (movBrazosChicken > 30.0f) dirChicken2 = -0.5f;
	else if (movBrazosChicken < -30.0f) dirChicken2 = 0.5f;
	if (movPiernaChicken > 20.0f) dirChicken3 = -0.7f;
	else if (movPiernaChicken < -20.0f) dirChicken3 = 0.7f;

	// Movimiento con teclado
	dirChicken = glm::vec3(sin(glm::radians(giroChicken)), 0.0f, cos(glm::radians(giroChicken)));

	if (mainWindow.getsKeys()[GLFW_KEY_UP])
	{
		posChicken -= dirChicken * speedChicken;
	}
	if (mainWindow.getsKeys()[GLFW_KEY_DOWN])
	{
		posChicken += dirChicken * speedChicken;
	}
	if (mainWindow.getsKeys()[GLFW_KEY_LEFT])
	{
		giroChicken += speedGiroChicken;
	}
	if (mainWindow.getsKeys()[GLFW_KEY_RIGHT])
	{
		giroChicken -= speedGiroChicken;
	}

	// Movimiento de las alas de la mariposa (ciclico)
	tiempo += deltaTiempo;
	angulo = 35.0f * sin(speedAleteo * tiempo);

	// Movimiento de los hexagonos del cielo
	posHexagonos += speedHexagonos;
	if (posHexagonos < -637.0f)
	{
		speedHexagonos = 0.25;
	}
	else if (posHexagonos > 0.0f)
	{
		speedHexagonos = -0.25;
	}
}

// Función - Animacion de la Entrada Principal
void animationEntrada()
{
	if (mainWindow.getsKeys()[GLFW_KEY_O])
	{
		if (!animandoPuertas)
		{
			puertasAbiertas = !puertasAbiertas;
			animandoPuertas = true;
			mainWindow.getsKeys()[GLFW_KEY_O] = false;
		}
	}
	if (animandoPuertas)
	{
		if (puertasAbiertas)  // Abrir puertas
		{
			if (rotacionPuertaDerecha < 90.0f)
			{
				rotacionPuertaDerecha += 2.0f * deltaTime;
				if (rotacionPuertaDerecha > 90.0f)
					rotacionPuertaDerecha = 90.0f;
			}

			if (rotacionPuertaDerecha >= 90.0f)
			{
				animandoPuertas = false;
			}
		}
		else  // Cerrar puertas
		{
			if (rotacionPuertaDerecha > 0.0f)
			{
				rotacionPuertaDerecha -= 2.0f * deltaTime;
				if (rotacionPuertaDerecha < 0.0f)
					rotacionPuertaDerecha = 0.0f;
			}

			if (rotacionPuertaDerecha <= 0.0f)
			{
				animandoPuertas = false;
			}
		}
	}
}

// Funcion - Audio
void initAudio() {
	// Inicializar engine una sola vez
	ma_engine_init(NULL, &engine);

	// Audio de fondo (loop)
	ma_sound_init_from_file(&engine, "Audios/Audio Maya.mp3",
	MA_SOUND_FLAG_DECODE, NULL, NULL, &ambiente);
	ma_sound_set_looping(&ambiente, MA_TRUE);
	ma_sound_set_volume(&ambiente, 0.02f);
	ma_sound_start(&ambiente);

	// Sonidos por zonas
	ma_sound_init_from_file(&engine, "Audios/Mucha Lucha - Theme Song.mp3", 0, NULL, NULL, &soundQ[0]);
	ma_sound_init_from_file(&engine, "Audios/Chilly Willy - Theme Song.mp3", 0, NULL, NULL, &soundQ[1]);
	ma_sound_init_from_file(&engine, "Audios/Chicken Little - Ain't No Mountain High.mp3", 0, NULL, NULL, &soundQ[2]);
	ma_sound_init_from_file(&engine, "Audios/My Neighbor Totoro - Theme Song.mp3", 0, NULL, NULL, &soundQ[3]);
	ma_sound_init_from_file(&engine, "Audios/Campana - Ring.mp3", 0, NULL, NULL, &soundQ[4]);

	for (int i = 0; i < 5; i++) {
		ma_sound_stop(&soundQ[i]);
		ma_sound_set_volume(&soundQ[i], 0.0f);
		ma_sound_set_looping(&soundQ[i], MA_FALSE);
	}
}

// Funcion - Obtener Audio por zona
int getQuadrant(glm::vec3 pos) {
	if (pos.x >= 375 && pos.z >= 375) return 0; // Mucha lucha
	if (pos.x < -375 && pos.z >= 375) return 1; // Chilly Willy
	if (pos.x < -375 && pos.z < -375) return 2; // Chicken Little
	if (pos.x > 375 && pos.z < -375) return 3; // Totoro
	else return 4;
}


// -------------------------------------------------------------------------- Main
int main()
{
	initAudio();
	auto start = std::chrono::high_resolution_clock::now();

	// Iniciamos el contexto
	mainWindow = Window(1366, 768); // Resolución: 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	// Recursos
	ResourceManager resources;

	CreatePrimitives();				// Cargamos las primitivas en meshList
	CreateShaders();				// Creamos los shaders
	SetLights();					// Creamos las luces

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);
	Material_Chicken_piel = Material(0.6f, 16);
	Material_Chicken_piernas = Material(0.8f, 32);


	auto end = std::chrono::high_resolution_clock::now(); // Fin del cronómetro
	std::chrono::duration<double> loadTime = end - start;
	std::cout << "Tiempo total de carga: " << loadTime.count() << " segundos\n";

	// Variables uniform
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, 
			uniformSpecularIntensity = 0, uniformShininess = 0, uniformColor = 0, uniformTextureOffset = 0;

	// Vector de proyección
	glm::mat4 projection = glm::perspective(
			45.0f,				// Ángulo de vision
			(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), // Aspecto
			0.1f, 2800.0f);		// Alcance: cerca | lejos
	
	// Matrices para los modelos
	glm::mat4 modelBase(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelZoo(1.0);		// Matriz para los animales
	glm::mat4 modelChicken(1.0);	// Matriz para los modelos de Chicken Little
	glm::mat4 modelTotoro(1.0);		// Matriz para los modelos de Totoro
	glm::mat4 modelChilly(1.0);		// Matriz para los modelos de Chilly Willy
	glm::mat4 modelRikoche(1.0);	// Matriz para los modelos de Rikoche

	// Posicion del origen
	glm::vec3 origen = glm::vec3(0.0f, 0.0f, 0.0f);

	// Posiciones de cada cuadrante
	glm::vec3 origenChicken = glm::vec3(-680.0f, 0.0f, -695.0f);// Cuadrante superior izq
	glm::vec3 origenChilly = glm::vec3(-690.0f, 0.0f, 690.0f);	// Cuadrante inferior izq
	glm::vec3 origenRikoche = glm::vec3(720.0f, 0.0f, 700.0f);	// Cuadrante inferior der
	glm::vec3 origenTotoro = glm::vec3(690.0f, 0.0f, -690.0f);	// Cuadrante superior der

	// Posiciones para los animales
	std::vector<glm::vec3> posAnimals;

	// Variables auxiliares
	glm::vec3 color = glm::vec3(1.0f);		// Para mandar un color
	glm::vec3 pos = glm::vec3(1.0f);		// Para mover los modelos
	glm::vec3 auxscal = glm::vec3(1.0f);	// Para escalar con un vector
	float scal = 1.0f;						// Para escalar los modelos
	float rot = 0.0f;						// Para rotar los modelos
	glm::vec3 posLight = glm::vec3(0);
	
	// Variables auxiliares para animaciones
	glm::mat4 model(1.0);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	// Ajustes para las camaras
	PovAereaCamera = Camera(glm::vec3(0.0f, 300.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -90.0f, 5.0f, 0.45f);
	PovSitesCamera = Camera(glm::vec3(0.0f, 50.0f, 300.0f), glm::vec3(0.0f, 1.0f, 0.0f), -180.0f, 0.0f, 5.0f, 0.45f);
	Pov3erCamera = Camera(posChicken, glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 4.0f, 0.45f);

	// ------------------------------------------------------------------- KeyFrames
	
	// Trayectoria de vuelo de la mariposa
	// Keyframes de la mariposa (trayectoria más amplia)
	anim.KeyFrame[0].gx = 100.0f; anim.KeyFrame[0].gy = 50.0f;  anim.KeyFrame[0].gz = 0.0f;    anim.KeyFrame[0].angle = 90.0f;
	anim.KeyFrame[1].gx = 150.0f; anim.KeyFrame[1].gy = 80.0f;  anim.KeyFrame[1].gz = 30.0f;   anim.KeyFrame[1].angle = 100.0f;
	anim.KeyFrame[2].gx = 200.0f; anim.KeyFrame[2].gy = 120.0f; anim.KeyFrame[2].gz = 20.0f;   anim.KeyFrame[2].angle = 110.0f;
	anim.KeyFrame[3].gx = 180.0f; anim.KeyFrame[3].gy = 160.0f; anim.KeyFrame[3].gz = -10.0f;  anim.KeyFrame[3].angle = 120.0f;
	anim.KeyFrame[4].gx = 130.0f; anim.KeyFrame[4].gy = 140.0f; anim.KeyFrame[4].gz = -30.0f;  anim.KeyFrame[4].angle = 130.0f;
	anim.KeyFrame[5].gx = 120.0f; anim.KeyFrame[5].gy = 90.0f;  anim.KeyFrame[5].gz = 0.0f;    anim.KeyFrame[5].angle = 140.0f;
	anim.KeyFrame[6].gx = 110.0f; anim.KeyFrame[6].gy = 70.0f;  anim.KeyFrame[6].gz = 10.0f;   anim.KeyFrame[6].angle = 150.0f;
	anim.KeyFrame[7].gx = 100.0f; anim.KeyFrame[7].gy = 50.0f;  anim.KeyFrame[7].gz = 0.0f;    anim.KeyFrame[7].angle = 90.0f;

	anim.FrameIndex = 8;

	anim.Instrucciones();
	anim.LoadFrames();

	while (!mainWindow.getShouldClose()) // Loop Principal 
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Recibir eventos del usuario 
		glfwPollEvents();
		
		// Modo de Camara

		offsetChicken_camera = glm::vec3(0.0f, 100.0f, 170.0f);

		if (mainWindow.getsKeys()[GLFW_KEY_C] && !cPressed) {
			cPressed = true;

			if (cameraActual == &Pov3erCamera)
				cameraActual = &PovAereaCamera;
			else if (cameraActual == &PovAereaCamera)
				cameraActual = &PovSitesCamera;
			else if (cameraActual == &PovSitesCamera)
				cameraActual = &Pov3erCamera;
		}
		if (!mainWindow.getsKeys()[GLFW_KEY_C])
			cPressed = false;

		if (cameraActual == &PovAereaCamera) {
			cameraActual->aerialKeyControl(mainWindow.getsKeys(), deltaTime, 800.0f);
		}
		else if (cameraActual == &Pov3erCamera) {
			cameraActual->setPosYaw(posChicken, giroChicken, offsetChicken_camera);
		}
		else if (cameraActual == &PovSitesCamera) {
			cameraActual->setPosYaw(origenChicken + glm::vec3(375, 200, 375), 45, offsetChicken_camera);
			cameraActual->setPosYaw(origenChilly + glm::vec3(375, 200, -375), 90, offsetChicken_camera);
			cameraActual->setPosYaw(origenRikoche + glm::vec3(-375, 200, -375), 135, offsetChicken_camera);
			cameraActual->setPosYaw(origenTotoro + glm::vec3(-375, 200, 375), 180, offsetChicken_camera);
		}
		else {
			cameraActual->keyControl(mainWindow.getsKeys(), deltaTime);
			cameraActual->mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}

		// Audio por cada cuadrante
		glm::vec3 camPos = 100.0f * cameraActual->getCameraPosition();
		int q = getQuadrant(camPos);

		if (q != currentQuadrant) {
			// Apagar el sonido anterior (si existe)
			if (currentQuadrant != -1) {
				ma_sound_stop(&soundQ[currentQuadrant]);
				ma_sound_stop(&ambiente);
			}
			// Ajustar volumen del nuevo
			ma_sound_set_volume(&soundQ[q], 0.25f);

			// Reproducir sonido del nuevo cuadrante
			ma_sound_start(&soundQ[q]);

			currentQuadrant = q;
		}

		// Limpiamos la window
		mainWindow.Clear();

		// Dibujamos el Skybox 

		cieloTime += deltaTime;

		if (cieloTime >= cielo_max) {
			cieloTime = 0.0f;
			cieloActual = (cieloActual + 1) % 5;
		}

		resources.skybox[cieloActual].DrawSkybox(cameraActual->calculateViewMatrix(), projection);

		// Configuracion para la iluminación


		// Info en el Shader 
		shaderList[0].UseShader();
		uniformModel =				shaderList[0].GetModelLocation();				// modelo
		uniformProjection =			shaderList[0].GetProjectionLocation();			// proyección
		uniformView =				shaderList[0].GetViewLocation();				// vista
		uniformEyePosition =		shaderList[0].GetEyePositionLocation();			// -
		uniformColor =				shaderList[0].getColorLocation();				// color
		uniformSpecularIntensity =	shaderList[0].GetSpecularIntensityLocation();	// intensidad especular
		uniformShininess =			shaderList[0].GetShininessLocation();			// brillo
		uniformTextureOffset =		shaderList[0].GetTextureOffsetLocation();		// offset de textura

		shaderList[0].SetDirectionalLight(&mainLight[cieloActual]);					// iluminación direccional
		shaderList[0].SetPointLights(PL_0, pointLightCount);						// iluminación pointlight

		if (mainWindow.getlightOff() < 1.0f)
		{
			shaderList[0].SetSpotLights(SP_lamparas, 0);					// iluminación spotlight
		}
		else
		{
			shaderList[0].SetSpotLights(SP_lamparas, spotLightCount);		// iluminación spotlight
		}
		

		glUniformMatrix4fv(uniformProjection,	1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView,			1, GL_FALSE, glm::value_ptr(cameraActual->calculateViewMatrix()));
		glUniform3f(uniformEyePosition, cameraActual->getCameraPosition().x, cameraActual->getCameraPosition().y, cameraActual->getCameraPosition().z);

		// Inicializar textureOffset con valor por defecto (0, 0) para modelos sin animación de textura
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));


		// ------------------------------------------------------------------ === Animaciones ===
		anim.InputKeyframes(mainWindow.getsKeys());
		anim.Animate();

		animationEntrada();
		animationsChickenLittle();
		animationsTotoro();

		// ------------------------------------------------------------------ === Primitivas ===

		// Mesh [0]: Suelo
		modelBase = glm::mat4(1.0);
		pos = glm::vec3(0.0f, -0.05f, 0.0f);
		auxscal = glm::vec3(100.0f, 1.0f, 100.0f);

		RenderMeshWithTexture(meshList[0], color, pos, auxscal, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess,
			&Material_opaco, &resources.croquisTexture);

		// ------------------------------------------------------------------ === Arco ===

		// Pilares
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.6f, 850.0f));
		model = glm::scale(model, glm::vec3(8.0f, 3.0f, 5.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resources.Entrada_M.RenderModel();

		// Puerta Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-20.0f, 15.0f, 0.0f));
		model = glm::rotate(model, rotacionPuertaDerecha * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resources.EntradaPuertaDerecha_M.RenderModel();

		// Puerta Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(19.0f, 15.0f, 0.0f));
		model = glm::rotate(model, -rotacionPuertaDerecha * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resources.EntradaPuertaIzquierda_M.RenderModel();

		// Arco - cartel
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 52.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resources.Cartel_M.RenderModel();

		// ------------------------------------------------------------------ === Arena Central ===
		
		// Arena Central
		modelBase = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelBase, uniformModel, origen, pos, resources.ArenaCentral);

		// Guardianes
		modelBase = glm::mat4(1);
		pos = glm::vec3(0.0f, -17.0f, 0.0f);
		modelBase = glm::rotate(modelBase, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		RenderModel(modelBase, uniformModel, origen, pos, resources.Guardianes_M);

		// Tianguis
		modelBase = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelBase, uniformModel, origen, pos, resources.Tianguis_M);

		// NPCs
		modelBase = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelBase, uniformModel, origen, pos, resources.NPCS);

		// Iluminaria
		posLamp1 = glm::vec3(-342, 280, 250); // verde
		SP_lamparas[0].SetPos(glm::vec3(posLamp1 + posLight));

		posLamp1 = glm::vec3(346, 280, 250);
		SP_lamparas[1].SetPos(glm::vec3(posLamp1 + posLight));

		posLamp1 = glm::vec3(346, 280, -253);
		SP_lamparas[2].SetPos(glm::vec3(posLamp1 + posLight));

		posLamp1 = glm::vec3(-346, 280, -245);
		SP_lamparas[3].SetPos(glm::vec3(posLamp1 + posLight));

		modelBase = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelBase, uniformModel, origen, pos, resources.Iluminaria);

		// ------------------------------------------------------------------ === Zoológico ===
		
		// Animales
		modelZoo = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelZoo, uniformModel, origen, pos, resources.Animales);

		// ------------------------------------------------------------------ === Universo de Chicken Little ===
		
		// Piramide de Chicken Little
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.PiramideChicken);

		// Edificios
		std::vector<glm::vec3> posEdificios = {
			{-100.0f, 0.0f, 250.0f},	// Casa
			{380.0f, 0.0f, -120.0f},	// Ayuntamiento
			{360.0f, 0.0f, -120.0f},	// Estacion de bomberos
			{350.0f, 0.0f, -120.0f},	// Torre de agua
			{-20.0f, 0.0f, 400.0f},		// Edificio naranja
			{-20.0f, 0.0f, 420.0f},		// Edificio Tienda
			{-20.0f, 0.0f, 440.0f},		// Edificio Oaks
		};
		modelChicken = glm::mat4(1.0);
		for (size_t i = 0; i < resources.EdificiosLittle.size(); i++) {
			RenderModel(modelChicken, uniformModel, origenChicken, glm::vec3(0.0f, 0.0f, 0.0f), resources.EdificiosLittle[i]);
		}

		// Arboles
		modelChicken = glm::mat4(1.0);
		for (size_t i = 0; i < resources.Arboles.size(); i++) {
			RenderModel(modelChicken, uniformModel, origen, glm::vec3(0.0f, 0.0f, 0.0f), resources.Arboles[i]);
		}

		// Mariposa Azul
		modelChicken = glm::mat4(1.0);
		modelChicken = glm::translate(modelChicken, origenChicken + glm::vec3(100.0f + anim.gx, 50.0f + anim.gy, 256.0f + anim.gz));
		modelChicken = glm::rotate(modelChicken, glm::radians(anim.angle), glm::vec3(0, 1, 0));
		RenderModel(modelChicken, uniformModel, glm::vec3(0), glm::vec3(0), resources.MariposaAzul);
		modelaux = modelChicken;

		// Mariposa Azul - ala izquierda
		modelChicken = modelaux;
		modelChicken = glm::translate(modelChicken, glm::vec3(1.0f, 0.0f, 0.0f));
		modelChicken = glm::rotate(modelChicken, glm::radians(-angulo), glm::vec3(0, 1, 0));
		RenderModel(modelChicken, uniformModel, glm::vec3(0), glm::vec3(0), resources.MariposaAzul_AlaIzquierda);

		// Mariposa Azul - ala derecha
		modelChicken = modelaux;
		modelChicken = glm::translate(modelChicken, glm::vec3(-1.0f, 0.0f, 0.0f));
		modelChicken = glm::rotate(modelChicken, glm::radians(angulo), glm::vec3(0, 1, 0));
		RenderModel(modelChicken, uniformModel, glm::vec3(0), glm::vec3(0), resources.MariposaAzul_AlaDerecha);

		// Cielo Hexagonal
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.CieloHexagonal);

		// Hexagonos
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, posHexagonos, 0.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Hexagono);

		// Chicken Little - AVATAR
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		modelChicken = glm::translate(modelChicken, posChicken);
		modelChicken = glm::rotate(modelChicken, glm::radians(180 + giroChicken), glm::vec3(0.0f, 1.0f, 0.0f));
		Material_Chicken_piel.UseMaterial(uniformSpecularIntensity, uniformShininess);
		RenderModel(modelChicken, uniformModel, glm::vec3(0), glm::vec3(0), resources.ChickenLittle_cuerpo);

		modelaux = modelChicken;
		// brazo izquierdo
		modelChicken = modelaux;
		pos = glm::vec3(5.53f, 24.90f, 0.0f);
		Material_Chicken_piel.UseMaterial(uniformSpecularIntensity, uniformShininess);
		RenderModelRotate(modelChicken, uniformModel, glm::vec3(0), pos, movBrazosChicken, glm::vec3(0, 1, 0), resources.ChickenLittle_brazoIzquierdo);

		// brazo derecho
		modelChicken = modelaux;
		pos = glm::vec3(-5.79f, 24.90f, 0.0f);
		Material_Chicken_piel.UseMaterial(uniformSpecularIntensity, uniformShininess);
		RenderModelRotate(modelChicken, uniformModel, glm::vec3(0), pos, movBrazosChicken, glm::vec3(0, 1, 0), resources.ChickenLittle_brazoDerecho);

		// pierna izquierda
		modelChicken = modelaux;
		pos = glm::vec3(3.62f, 11.19f, 0.0f);
		Material_Chicken_piernas.UseMaterial(uniformSpecularIntensity, uniformShininess);
		RenderModelRotate(modelChicken, uniformModel, glm::vec3(0), pos, movPiernaChicken, glm::vec3(1, 0, 0), resources.ChickenLittle_pierna);

		// pierna derecha
		modelChicken = modelaux;
		pos = glm::vec3(-3.73f, 11.19f, 0.0f);
		Material_Chicken_piernas.UseMaterial(uniformSpecularIntensity, uniformShininess);
		RenderModelRotate(modelChicken, uniformModel, glm::vec3(0), pos, -movPiernaChicken, glm::vec3(1, 0, 0), resources.ChickenLittle_pierna);

		// Abby Patosa
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.AbbyPatosa);

		// Runt
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.Runt);

		// Camion de Bomberos
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.CamionBomberos);

		// PezOutWater
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.PezOutWater);

		// Kirby
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.Kirby);

		// Buck Gallo
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.BuckGallo);

		// Melvin
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.Melvin);

		// Tina
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.Tina);

		// Ace
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.Ace);

		// Robot Alien
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.RobotAlien);

		// The Dog
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.TheDog);
	
		// ------------------------------------------------------------------ === Universo de Chilly Willy ===

		// Piramide de Chilly Willy
		modelChilly = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.PiramideChilly);

		// Chilly Willy
		modelChilly = glm::mat4(1.0);
		pos = glm::vec3(35.0f, 0.0f, 20.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Chilly_Willy);

		// Lago de hielo
		modelChilly = glm::mat4(1.0);
		pos = glm::vec3(35.0f, 0.0f, 20.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Lago);

		// Árbol de hielo
		modelChilly = glm::mat4(1.0);
		pos = glm::vec3(35.0f, 0.0f, 20.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Arbol_Hielo);

		// Árbol de hielo 2
		modelChilly = glm::mat4(1.0);
		pos = glm::vec3(35.0f, 0.0f, 20.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Arbol_Hielo);

		// Iglu
		modelChilly = glm::mat4(1.0);
		pos = glm::vec3(35.0f, 0.0f, 20.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Iglu);

		// Anuncio
		modelChilly = glm::mat4(1.0);
		pos = glm::vec3(35.0f, 0.0f, 20.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Anuncio);

		// Animalitos
		modelChilly = glm::mat4(1.0);
		pos = glm::vec3(35.0f, 0.0f, 20.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Animalitos);

		// ------------------------------------------------------------------ === Universo de Rikoche === 
		
		// Piramide de Rikoche
		modelRikoche = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelRikoche, uniformModel, origenRikoche, pos, resources.PiramideRikoche);

		// Rikochet
		modelRikoche = glm::mat4(1.0);
		pos = glm::vec3(-150.0f, 0.0f, -150.0f);
		RenderModel(modelRikoche, uniformModel, origenRikoche, pos, resources.Rikoche);

		// Lucha Ville
		modelRikoche = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelRikoche, uniformModel, origen, pos, resources.LuchaVille_M);

		// Arbolitos
		modelRikoche = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelRikoche, uniformModel, origen, pos, resources.ArbolitosRikochet);

		// ------------------------------------------------------------------ === Universo de Totoro ===	

		// Piramide de Totoro
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.PiramideTotoro);

		// Totoro - cuerpo
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		modelTotoro = glm::translate(modelTotoro, glm::vec3(0, 50, movTotoro));
		RenderModel(modelTotoro, uniformModel, glm::vec3(0), glm::vec3(0), resources.Totoro);
		modelaux = modelTotoro;

		// Totoro - brazo izquierdo
		modelTotoro = modelaux;
		pos = glm::vec3(21.4f, 44.60f, 0.0f);
		RenderModelRotate(modelTotoro, uniformModel, glm::vec3(0), pos, movBrazosTotoro, glm::vec3(0, 1, 0), resources.TotoroBrazoIzquierdo);

		// Totoro - brazo derecho
		modelTotoro = modelaux;
		pos = glm::vec3(-19.8f, 44.83f, 0.0f);
		RenderModelRotate(modelTotoro, uniformModel, glm::vec3(0), pos, movBrazosTotoro, glm::vec3(0, 1, 0), resources.TotoroBrazoDerecho);
		
		// Totoro - pierna izquierda
		modelTotoro = modelaux;
		pos = glm::vec3(8.91f, 4.76f, 0.0f);
		RenderModelRotate(modelTotoro, uniformModel, glm::vec3(0), pos, movPiernasTotoro, glm::vec3(1, 0, 0), resources.TotoroPieIzquierdo);

		// Totoro - pierna derecha
		modelTotoro = modelaux;
		pos = glm::vec3(-9.02f, 4.84f, 0.0f);
		RenderModelRotate(modelTotoro, uniformModel, glm::vec3(0), pos, -movPiernasTotoro, glm::vec3(1, 0, 0), resources.TotoroPieDerecho);

		// Totoro mediano (Chū-Totoro)
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro_mediano);

		// Totoro chiquito (Chibi-Totoro)
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro_chiquito);

		// Conejo
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.conejo_universo_totoro);

		// Búho
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 200.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origenTotoro, pos, resources.buho_universo_totoro);

		// Tortuga
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.tortuga_universo_totoro);

		// Gato
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.gato_universo_totoro);

		// ------------------------------------------------------------------ === Enable - Modelos con Blending (transparencia o traslucidez) ===
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// ------------------------------------------------------------------ Arco - Letrero

		// Animacion de Textura
		toffsetLetrero += 0.001;
		toffsetLetrerov = 0.000;
		//para que no se desborde la variable
		if (toffsetLetrero > 1.0)
			toffsetLetrero = 0.0;

		toffset = glm::vec2(toffsetLetrero, toffsetLetrerov);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 150.0f, 870.0f));
		model = glm::scale(model, glm::vec3(400.0f, 190.0f, 300.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		resources.LetreroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		glDisable(GL_BLEND);
		// ------------------------------------------------------------------ === Disable - Modelos con Blending (transparencia o traslucidez) ===

		// ------------------------------------------------------------------ End of program 
		glUseProgram(0);
		mainWindow.swapBuffers();
	}	

	ma_engine_uninit(&engine);
	return 0;
}