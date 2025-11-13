
#define STB_IMAGE_IMPLEMENTATION	//	Para cargar imágenes
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

// ------------------------------------------------------------------ Ventana

Window mainWindow;

std::vector<Mesh*> meshList;		// Vector de Meshs
std::vector<Shader> shaderList;		// Vector de Shaders

// ------------------------------------------------------------------ Camara

Camera camera;

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

DirectionalLight mainLight;					// Luz Direccional (Sol)
PointLight PL_0[MAX_POINT_LIGHTS];			// Arreglos de luces PointLight
SpotLight PL_1[MAX_SPOT_LIGHTS];			//.
SpotLight SP_RGB[MAX_SPOT_LIGHTS];			// Arreglos de luces SpotLight
SpotLight SP_lamparas[MAX_SPOT_LIGHTS];		//.

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

glm::vec3 posChicken = glm::vec3(0.0f, 0.0f, 0.0f);
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
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(
		0.83f, 0.78f, 0.92f,	// Color
		1.0f, 0.3f,				// Intensidades ambiente y difusa más bajas
		-1.0f, 0.0f, 0.0f		// Dirección
	);

	// Arreglo 1 para cambio de luces
	// Verde
	SP_lamparas[0] = SpotLight(0.0f, 1.0f, 0.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		20.0f, 15.0f, 0.0f,                     // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.2f, 0.0f, 0.0f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Azul
	SP_lamparas[1] = SpotLight(0.0f, 0.0f, 1.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		10.0f, 15.0f, 0.0f,                     // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.0f, 0.01f, 0.0f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Roja
	SP_lamparas[2] = SpotLight(1.0f, 0.0f, 0.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		0.0f, 15.0f, 0.0f,                      // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.0f, 0.0f, 0.01f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Blanca
	SP_lamparas[3] = SpotLight(1.0f, 1.0f, 1.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		-10.0f, 15.0f, 0.0f,                    // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.2f, 0.01f, 0.01f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
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

// Función - Animacion Totoro (caminata)
void animatedTotoro()
{
	movTotoro += dirTotoro1;
	movBrazosTotoro += dirTotoro2;
	movPiernasTotoro += dirTotoro3;

	// límites totoro
	if (movTotoro > 50.0f) dirTotoro1 = -0.25f;
	else if (movTotoro < -50.0f) dirTotoro1 = 0.25f;

	// límites brazos
	if (movBrazosTotoro > 10.0f) dirTotoro2 = -0.5f;
	else if (movBrazosTotoro < -10.0f) dirTotoro2 = 0.5f;

	// límites piernas
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
	if (movBrazosChicken > 10.0f) dirChicken2 = -0.5f;
	else if (movBrazosChicken < -10.0f) dirChicken2 = 0.5f;
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

// -------------------------------------------------------------------------- Main
int main()
{
	auto start = std::chrono::high_resolution_clock::now(); // Inicio del cronómetro

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
	
	// Variables auxiliares para animaciones
	glm::mat4 model(1.0);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	// Ajustes para la cámara
	camera = Camera(
		//origenChicken + glm::vec3(325.0f, 50.0f, 325.0f),// Posicion inicial
		origen + glm::vec3(0.0f, 30.0f, 1050.0f),// Posicion inicial
		glm::vec3(0.0f, 1.0f, 0.0f),			// Direccion de nuestro "arriba"
		//-135.0f, 0.0f,						// Rotacion horizontal | Rotacion vertical
		-90.0f, 0.0f,						// Rotacion horizontal | Rotacion vertical
		4.0f, 0.45f);					// Velocidad de movimiento | Velocidad de rotacion
	
	// ------------------------------------------------------------------- KeyFrames
	// Configuración de KeyFrames del avión
	anim.KeyFrame[0].gx = 100.0f;
	anim.KeyFrame[0].gy = 50.0f;
	anim.KeyFrame[0].gz = 0.0f;
	anim.KeyFrame[0].angle = 90.0f;

	// Se indica el total de keyframes cargados
	anim.FrameIndex = 5;

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
		/*camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());*/

		// Control de modo de cámara
		if (mainWindow.getsKeys()[GLFW_KEY_Z])
		{
			camera.setAerialMode(true);
			camera.setCornerCameraMode(false); // Desactivar modo de esquinas
			printf("Camara aerea activada\n");
			mainWindow.getsKeys()[GLFW_KEY_Z] = false;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_X])
		{
			camera.setAerialMode(false);
			camera.setCornerCameraMode(false); // Desactivar modo de esquinas
			printf("Camara normal activada\n");
			mainWindow.getsKeys()[GLFW_KEY_X] = false;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_C])
		{
			camera.setCornerCameraMode(true);
			currentCornerCamera = 0;
			cornerCameraTimer = 0.0f;
			camera.updateCornerCamera(currentCornerCamera, origen);
			printf("Modo de camaras de esquina activado\n");
			mainWindow.getsKeys()[GLFW_KEY_C] = false;
		}

		// Actualizar cámaras de esquina si está activado el modo
		if (camera.isCornerCameraMode())
		{
			cornerCameraTimer += deltaTime;
			if (cornerCameraTimer >= cornerCameraInterval)
			{
				cornerCameraTimer = 0.0f;
				currentCornerCamera = (currentCornerCamera + 1) % 4; // Rotar entre 0, 1, 2, 3
				camera.updateCornerCamera(currentCornerCamera, origen);
				printf("Cambiando a camara de esquina %d\n", currentCornerCamera + 1);
			}
		}
		else
		{
			// Solo permitir control manual si no está en modo de cámaras de esquina
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}


		// Limpiamos la window
		mainWindow.Clear();

		// Dibujamos el Skybox 
		resources.skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		// Configuracion para la iluminación
		
		// código...
		
		// KEYFRAMES -------------------------------------------------------------
		anim.InputKeyframes(mainWindow.getsKeys());
		anim.Animate();

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

		shaderList[0].SetDirectionalLight(&mainLight);								// iluminación direccional
		shaderList[0].SetPointLights(PL_0, pointLightCount);						// iluminación pointlight
		shaderList[0].SetSpotLights(SP_lamparas, spotLightCount);					// iluminación spotlight

		glUniformMatrix4fv(uniformProjection,	1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView,			1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Inicializar textureOffset con valor por defecto (0, 0) para modelos sin animación de textura
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		// ------------------------------- ANIMACION DE PUERTA ----------------------------------------------

		  // DETECCIÓN DE TECLA 'O' PARA ABRIR/CERRAR PUERTAS
		if (mainWindow.getsKeys()[GLFW_KEY_O])
		{
			if (!animandoPuertas)
			{
				puertasAbiertas = !puertasAbiertas;
				animandoPuertas = true;
				mainWindow.getsKeys()[GLFW_KEY_O] = false;
			}
		}

		// ------------------------------- ANIMACION DE PUERTAS ----------------------------------------------
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
		modelaux = model; // Guardamos la transformación base CON escala
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

		// ------------------------------------------------------------------ === Zoológico ===
		
		// Animales
		modelZoo = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelZoo, uniformModel, origen, pos, resources.Animales);

		// ------------------------------------------------------------------ === Universo de Chicken Little ===
		
		// Llamadas a funciones
		animationsChickenLittle();

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
		//for (size_t i = 0; i < resources.Arboles.size(); i++) {
			RenderModel(modelChicken, uniformModel, origenChicken, glm::vec3(0.0f, 0.0f, 0.0f), resources.Arboles[1]);
		//}

		// Mariposa Azul
		modelChicken = glm::mat4(1.0);
		modelChicken = glm::translate(modelChicken, glm::vec3(0.0f + anim.gx, 0.0f + anim.gy, 0.0f + anim.gz));
		modelChicken = glm::rotate(modelChicken, glm::radians(anim.angle), glm::vec3(0, 1, 0));
		RenderModel(modelChicken, uniformModel, glm::vec3(0), glm::vec3(0), resources.MariposaAzul);
		modelaux = modelChicken;

		tiempo += deltaTiempo;
		angulo = 35.0f * sin(speedAleteo * tiempo);

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
		modelChicken = glm::translate(modelChicken, glm::vec3(0.0f, 0.0f, 1000.0f) + posChicken);
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
		pos = glm::vec3(280.0f, 0.0f, 280.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.AbbyPatosa);

		// Runt
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(265.0f, 0.0f, 265.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Runt);

		// PezOutWater
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(295.0f, 0.0f, 295.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.PezOutWater);

		// Kirby
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(300.0f, 0.0f, 300.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Kirby);

		// Buck Gallo
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(275.0f, 0.0f, 275.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.BuckGallo);

		// Melvin
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(305.0f, 0.0f, 305.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Melvin);

		// Tina
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(310.0f, 0.0f, 310.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Tina);

		// Ace
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(320.0f, 0.0f, 320.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Ace);

		// Robot Alien
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(320.0f, 0.0f, 320.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.RobotAlien);

		// The Dog
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(325.0f, 0.0f, 325.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.TheDog);
	
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
		
		// Llamadas a las animaciones
		animatedTotoro();

		// Piramide de Totoro
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, -45.0f, 0.0f);
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
		pos = glm::vec3(0.0f, -45.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro_mediano);

		// Totoro chiquito (Chibi-Totoro)
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, -45.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro_chiquito);

		// Conejo
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, -45.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.conejo_universo_totoro);

		// Búho
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, 200.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origenTotoro, pos, resources.buho_universo_totoro);

		// Tortuga
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, -45.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.tortuga_universo_totoro);

		// Gato
		modelTotoro = glm::mat4(1);
		pos = glm::vec3(0.0f, -45.0f, 0.0f);
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
	return 0;
}