#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();

	void setAerialMode(bool aerial);	// Cambiar modo de cámara
	bool isAerialMode();				// Verificar si está en modo aéreo

	void setCornerCameraMode(bool cornerMode);	// Activar/desactivar modo de c�maras de esquina
	bool isCornerCameraMode();					// Verificar si est� en modo de c�maras de esquina
	void updateCornerCamera(int cornerIndex, glm::vec3 center);	// Actualizar posici�n de c�mara de esquina

	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	bool aerialMode;		// Modo de cámara aérea
	GLfloat aerialHeight;	// Altura fija para modo aéreo
	bool cornerCameraMode;	// Modo de c�maras de esquina

	void update();
};

