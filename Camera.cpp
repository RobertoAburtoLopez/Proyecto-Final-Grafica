#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	aerialMode = false;			// Por defecto, cámara normal
	aerialHeight = 400.0f;		// Altura considerable para modo aéreo
	cornerCameraMode = false;	// Por defecto, modo de c�maras de esquina desactivado

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (aerialMode)
	{
		// En modo aéreo, solo se puede mover en X y Z (no en Y)
		glm::vec3 frontXZ = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
		glm::vec3 rightXZ = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

		if (keys[GLFW_KEY_W])
		{
			position += frontXZ * velocity;
		}

		if (keys[GLFW_KEY_S])
		{
			position -= frontXZ * velocity;
		}

		if (keys[GLFW_KEY_A])
		{
			position -= rightXZ * velocity;
		}

		if (keys[GLFW_KEY_D])
		{
			position += rightXZ * velocity;
		}

		// Mantener la altura fija
		position.y = aerialHeight;
	}
	else
	{

		if (keys[GLFW_KEY_W])
		{
			position += front * velocity;
		}

		if (keys[GLFW_KEY_S])
		{
			position -= front * velocity;
		}

		if (keys[GLFW_KEY_A])
		{
			position -= right * velocity;
		}

		if (keys[GLFW_KEY_D])
		{
			position += right * velocity;
		}
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}


glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::setAerialMode(bool aerial)
{
	aerialMode = aerial;
	if (aerialMode)
	{
		// Al activar modo aéreo, establecer la altura fija
		position.y = aerialHeight;
		// Apuntar hacia abajo para una vista aérea
		pitch = -45.0f;
		update();
	}
}

bool Camera::isAerialMode()
{
	return aerialMode;
}

void Camera::setCornerCameraMode(bool cornerMode)
{
	cornerCameraMode = cornerMode;
	if (cornerCameraMode)
	{
		aerialMode = false; // Desactivar modo a�reo al activar modo de esquinas
	}
}

bool Camera::isCornerCameraMode()
{
	return cornerCameraMode;
}

void Camera::updateCornerCamera(int cornerIndex, glm::vec3 center)
{
	// Posiciones de las 4 esquinas (basado en el suelo de 100x100 = 1000x1000 unidades)
	glm::vec3 cornerPositions[4] = {
		glm::vec3(-1000.0f, aerialHeight, -1000.0f),  // Esquina superior izquierda
		glm::vec3(1000.0f, aerialHeight, -1000.0f),   // Esquina superior derecha
		glm::vec3(-1000.0f, aerialHeight, 1000.0f),   // Esquina inferior izquierda
		glm::vec3(1000.0f, aerialHeight, 1000.0f)     // Esquina inferior derecha
	};

	// Establecer la posici�n de la c�mara en la esquina correspondiente
	position = cornerPositions[cornerIndex];

	// Calcular la direcci�n hacia el centro
	glm::vec3 direction = glm::normalize(center - position);
	front = direction;

	// Calcular yaw y pitch para que la c�mara apunte al centro
	yaw = glm::degrees(atan2(direction.z, direction.x));
	pitch = glm::degrees(asin(direction.y));

	update();
}

Camera::~Camera()
{
}
