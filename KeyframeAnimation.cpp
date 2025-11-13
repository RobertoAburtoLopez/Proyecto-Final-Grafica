#include "KeyframeAnimation.h"
#include <glfw3.h>

KeyframeAnimation::KeyframeAnimation()
    : gx(0.0f),
    gy(0.0f),
    gz(0.0f),
    angle(0.0f),
    FrameIndex(0),
    playIndex(0),
    i_max_steps(100),
    i_curr_steps(0),
    reproduciranimacion(0),
    habilitaranimacion(0),
    guardoFrame(0),
    reinicioFrame(0),
    ciclo(0), ciclo2(0), ciclo3(0), ciclo4(0), ciclo5(0), ciclo6(0),
    play(false),
    keyLPressed(false),
    keyPPressed(false),
    keySpacePressed(false)
{
}

void KeyframeAnimation::InputKeyframes(bool* keys) {
    // ----- TECLA P: habilitar guardado -----
    if (keys[GLFW_KEY_P]) {
        if (!keyPPressed) {
            keyPPressed = true;
            habilitaranimacion = 1;
            std::cout << "Guardado habilitado\n";
        }
    }
    else keyPPressed = false;

    // ----- TECLA L: guardar frame -----
    if (keys[GLFW_KEY_L]) {
        if (!keyLPressed && habilitaranimacion) {
            keyLPressed = true;
            SaveFrame();
            std::cout << "Frame " << FrameIndex - 1 << " guardado\n";
            habilitaranimacion = 0;
        }
    }
    else keyLPressed = false;

    // ----- Reproducción / reinicio animación -----
    if (keys[GLFW_KEY_SPACE]) {
        if (reproduciranimacion < 1) {
            if (!play && FrameIndex > 1) {
                ResetElements();      // reinicia posiciones a primer frame
                playIndex = 0;
                i_curr_steps = 0;
                Interpolation();      // prepara interpolación al siguiente frame
                play = true;
                reproduciranimacion++;
                habilitaranimacion = 0;
                std::cout << "Animación iniciada. Presiona 0 para reiniciar.\n";
            }
            else {
                play = false;         // pausa si ya estaba reproduciendo
            }
        }
    }

    // ----- Reinicio de animación / volver a habilitar -----
    if (keys[GLFW_KEY_0]) {
        if (reproduciranimacion > 0) {
            reproduciranimacion = 0;      // permite reproducir de nuevo
            ResetElements();              // reinicia posición al primer frame
            playIndex = 0;
            i_curr_steps = 0;
            std::cout << "Animación reiniciada y objeto en posición inicial.\n";
        }
    }

    // ----- Movimiento manual -----
    float delta = 0.1f;
    float rotSpeed = 2.0f;

    if (keys[GLFW_KEY_X]) {
        delta *= 5.0f;      // Modo X5
    }

    // Controles de movimiento
    if (keys[GLFW_KEY_1]) gx -= delta;  // Izquierda
    if (keys[GLFW_KEY_2]) gx += delta;  // Derecha
    if (keys[GLFW_KEY_3]) gy -= delta;  // Bajar
    if (keys[GLFW_KEY_4]) gy += delta;  // Subir
    if (keys[GLFW_KEY_5]) gz -= delta;  // Atras
    if (keys[GLFW_KEY_6]) gz += delta;  // Adelante
    if (keys[GLFW_KEY_7]) angle += delta;   // Rotar horario
    if (keys[GLFW_KEY_8]) angle -= delta;   // Rotar antihorario
    
}

void KeyframeAnimation::SaveFrame() {
    std::ofstream file("keyframes.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo abrir keyframes.txt\n";
        return;
    }

    file << "Frame " << FrameIndex << ":\n"
        << gx << " " << gy << " " << gz << " " << angle << "\n";
    file.close();

    // También guarda en memoria
    KeyFrame[FrameIndex].gx = gx;
    KeyFrame[FrameIndex].gy = gy;
    KeyFrame[FrameIndex].gz = gz;
    KeyFrame[FrameIndex].angle = angle;

    FrameIndex++;
}

void KeyframeAnimation::LoadFrames() {
    std::ifstream file("keyframes.txt");
    if (!file.is_open()) {
        std::cerr << "No se encontró keyframes.txt\n";
        return;
    }
    std::string line;
    int frameCount = 0;
    while (std::getline(file, line)) {
        if (line.find("Frame") != std::string::npos) continue;
        std::istringstream iss(line);
        float x, y, giro;
        if (iss >> x >> y >> giro) {
            int index = FrameIndex + frameCount;
            KeyFrame[index].gx = x;
            KeyFrame[index].gy = y;
            KeyFrame[index].angle = giro;
            frameCount++;
        }
    }
    file.close();
    FrameIndex += frameCount;
}

void KeyframeAnimation::ResetElements() {
    gx = KeyFrame[0].gx;
    gy = KeyFrame[0].gy;
    gz = KeyFrame[0].gz;
    angle = KeyFrame[0].angle;
}

void KeyframeAnimation::Interpolation() {
    KeyFrame[playIndex].gx_Inc = (KeyFrame[playIndex + 1].gx - KeyFrame[playIndex].gx) / i_max_steps;
    KeyFrame[playIndex].gy_Inc = (KeyFrame[playIndex + 1].gy - KeyFrame[playIndex].gy) / i_max_steps;
    KeyFrame[playIndex].gz_Inc = (KeyFrame[playIndex + 1].gz - KeyFrame[playIndex].gz) / i_max_steps;
    KeyFrame[playIndex].angle_Inc = (KeyFrame[playIndex + 1].angle - KeyFrame[playIndex].angle) / i_max_steps;
}

void KeyframeAnimation::Animate() {
    if (play) {
        if (i_curr_steps >= i_max_steps) {
            playIndex++;
            if (playIndex > FrameIndex - 2) {
                playIndex = 0;
                play = false;
            }
            else {
                i_curr_steps = 0;
                Interpolation();
            }
        }
        else {
            gx += KeyFrame[playIndex].gx_Inc;
            gy += KeyFrame[playIndex].gy_Inc;
            gz += KeyFrame[playIndex].gz_Inc;
            angle += KeyFrame[playIndex].angle_Inc;
            i_curr_steps++;
        }
    }
}

void KeyframeAnimation::Instrucciones() {
    std::cout <<    "\n     Controles de Animacion (keyframes):     \n";
    std::cout <<    "   Espacio: reproducir animacion\n";
    std::cout <<    "   (0) -> Reset animacion\n"
                    "   (L) -> Guardar frame\n"
                    "   (P) -> Habilitar guardar frame\n"
                    "   (1) -> Mover en -x | (2) -> Mover en x\n"
                    "   (3) -> Mover en -y | (4) -> Mover en y\n"
                    "   (5) -> Mover en -z | (6) -> Mover en z\n"
                    "   (7) -> Rotar horario | (8) -> Rotar antihorario\n"
        ;
}