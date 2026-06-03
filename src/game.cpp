/**
 * FITXER game.cpp
 * AUTOR Ferran Sanchez Bargas i Sabir Allouch El Imrani
 * DATA 02/06/2026
 * VERSIO 2.0 (Segon Lliurament - Motor i Grafics)
 * Implementacio de la classe Game per gestionar la logica en temps real i el bucle.
 */

 /**
 * 5 EJEMPLOS DE USO SOBRE COSAS APRENDIDAS EN CLASE (SEGUNDA ENTREGA)
 * * CONCEPTO 1: Encapsulament
 * La logica del juego interactua con m_board usando getCell() y setCell().
 * Game no necesita saber si Board guarda los datos en un vector o en un
 * array 1D. El estado interno del tablero esta totalmente protegido.
 * * CONCEPTO 2: Parametres i metodes const
 * Metodos como dump() y canMove() terminan en 'const', garantizando que
 * no modifican variables de clase por error, aumentando la seguridad.
 * * CONCEPTO 3: Sobrecarga de operadors
 * Hemos sobrecargado operator== para poder escribir 'game1 == game2' en los
 * tests unitarios y comparar su estado de forma nativa e intuitiva.
 * * CONCEPTO 4: Memoria dinamica (Gestor Maestro)
 * La clase Game es la "duena" de la memoria. Genera caramelos dinamicos (new Candy)
 * y, siguiendo las reglas, al aterrizar los pasa al Board pero cuando explotan
 * (o al cerrar el juego), Game se encarga de usar 'delete' sobre ellos.
 * * CONCEPTO 5: Bon disseny i estil (Responsabilidad Unica)
 * La funcion update() solo calcula matematicas y logica, y render() solo dibuja.
 * Nunca se mezclan datos con graficos en la misma funcion.
 */

#include "game.h"
#include <random>
#include <fstream>
#include "util.h"
#include "candy.h"

using namespace std;

/**
 * Game
 * Constructor del juego. Inicializa el tablero en memoria dinamica
 * y prepara las variables de la partida.
 */
Game::Game()
{
    m_board = new Board(); // Creacion dinamica del tablero
    m_score = 0;
    m_frames = 0;
    m_gameOver = false;

    // Inicializar controles a falso
    m_leftLastFrame = false;
    m_rightLastFrame = false;
    m_rotLastFrame = false;
    m_saveLastFrame = false;

    // Ponemos los caramelos que caen a nullptr por seguridad antes de crear el primero
    for (int i = 0; i < 3; i++) {
        m_fallingBlock[i] = nullptr;
    }

    // Spawneamos el primer bloque al iniciar
    spawnBlock();
}

/**
 * ~Game
 * Destructor del juego. Libera todos los caramelos vivos y destruye el tablero.
 */
Game::~Game()
{
    // 1. Limpiamos los caramelos que estaban cayendo pero no aterrizaron
    for (int i = 0; i < 3; i++) {
        if (m_fallingBlock[i] != nullptr) {
            delete m_fallingBlock[i];
            m_fallingBlock[i] = nullptr;
        }
    }

    // 2. Limpiamos TODOS los caramelos que quedaron fijos dentro del tablero
    if (m_board != nullptr) {
        for (int x = 0; x < m_board->getWidth(); x++) {
            for (int y = 0; y < m_board->getHeight(); y++) {
                Candy* c = m_board->getCell(x, y);
                if (c != nullptr) {
                    delete c;
                }
            }
        }
        // 3. Por ultimo, destruimos el tablero en si
        delete m_board;
    }
}

/**
 * spawnBlock
 * Genera el siguiente bloque de 3 piezas fuera de la pantalla en la columna 6.
 */
void Game::spawnBlock()
{
    // Si ya estamos en Game Over, no hacemos nada
    if (m_gameOver) return;

    // Generamos 3 colores aleatorios (de los 6 tipos)
    for (int i = 0; i < 3; i++) {
        CandyType r_type = static_cast<CandyType>(rand() % 6);
        m_fallingBlock[i] = new Candy(r_type);
    }

    // La regla dice que empieza en la columna 6 (indice 5) 
    m_fallingX = 5;
    // Empieza completamente fuera del tablero por arriba
    m_fallingY = -3;
}

/**
 * canMove
 * Metodo privado para detectar colisiones con paredes o piezas del tablero.
 */
bool Game::canMove(int dx, int dy) const
{
    // Verificamos el destino de los 3 caramelos del bloque
    for (int i = 0; i < 3; i++) {
        int nx = m_fallingX + dx;
        int ny = m_fallingY + dy + i;

        // Choque con las paredes laterales
        if (nx < 0 || nx >= m_board->getWidth()) return false;

        // Choque con el suelo del tablero
        if (ny >= m_board->getHeight()) return false;

        // Choque con otra pieza dentro del tablero
        // Ojo: Solo miramos la celda si el bloque ya ha entrado en la zona visible (ny >= 0)
        if (ny >= 0 && m_board->getCell(nx, ny) != nullptr) return false;
    }

    // Si no choca con nada, el movimiento es legal
    return true;
}

/**
 * landBlock
 * Fija el bloque en el tablero, comprueba el game over y detona las explosiones.
 */
void Game::landBlock()
{
    // REGLA GAME OVER: Si el bloque tiene que aterrizar pero todavia hay alguna
    // parte de el fuera del mapa (Y negativo), has perdido.
    for (int i = 0; i < 3; i++) {
        if (m_fallingY + i < 0) {
            m_gameOver = true;
            return;
        }
    }

    // Fijamos las piezas en la memoria del tablero
    for (int i = 0; i < 3; i++) {
        m_board->setCell(m_fallingBlock[i], m_fallingX, m_fallingY + i);
        // Quitamos la referencia al bloque cayendo, porque ahora son parte del tablero
        m_fallingBlock[i] = nullptr;
    }

    // Iniciamos la reaccion en cadena
    vector<Candy*> explotados;
    do {
        // Obtenemos los que han explotado gracias a nuestra logica de la Entrega 1
        explotados = m_board->explodeAndDrop();

        // 10 puntos por caramelo
        m_score += explotados.size() * 10;

        // MUY IMPORTANTE: Evitar memory leaks. Como Game creo las piezas (new), 
        // cuando explotan ya no sirven, asi que Game debe borrarlas (delete)
        for (Candy* c : explotados) {
            delete c;
        }
    } while (!explotados.empty()); // Si hubo explosiones, esto se repite para combos en cadena

    // Tras todo el jaleo de caidas, spawneamos el siguiente bloque
    spawnBlock();
}

/**
 * update
 * Motor del juego. Procesa teclado (inputs) y fisicas. Se ejecuta 60 veces/seg.
 */
void Game::update(const Controller& controller)
{
    // Si la partida ha terminado, paramos el motor
    if (m_gameOver) return;

    // --- REQUISITO GUARDAR PARTIDA (Tecla W = Boton 2) ---
    bool savePressed = controller.isKey2Pressed();
    if (savePressed && !m_saveLastFrame) {
        dump(getDataDirPath() + "save.txt"); // Guarda el estado del juego
    }
    m_saveLastFrame = savePressed;

    // --- MOVIMIENTO HORIZONTAL (Flechas Izq/Der) ---
    bool leftPressed = controller.isLeftPressed();
    if (leftPressed && !m_leftLastFrame) {
        if (canMove(-1, 0)) m_fallingX--;
    }
    m_leftLastFrame = leftPressed;

    bool rightPressed = controller.isRightPressed();
    if (rightPressed && !m_rightLastFrame) {
        if (canMove(1, 0)) m_fallingX++;
    }
    m_rightLastFrame = rightPressed;

    // --- ROTACION DEL BLOQUE (Tecla Q = Boton 1) ---
    // Rotamos los colores estilo A,B,C -> B,C,A
    bool rotPressed = controller.isKey1Pressed();
    if (rotPressed && !m_rotLastFrame) {
        Candy* temp = m_fallingBlock[0];
        m_fallingBlock[0] = m_fallingBlock[1];
        m_fallingBlock[1] = m_fallingBlock[2];
        m_fallingBlock[2] = temp;
    }
    m_rotLastFrame = rotPressed;

    // --- ACELERAR CAIDA (Flecha Abajo) ---
    // Si el jugador pulsa abajo, forzamos que sea el frame 60 para que caiga ya
    if (controller.isDownPressed()) {
        m_frames = 60;
    }

    // --- GRAVEDAD Y ATERRIZAJE ---
    m_frames++; // Aumenta 1 por frame

    // Si han pasado 60 frames (1 segundo), intenta bajar una posicion
    if (m_frames >= 60) {
        m_frames = 0; // Reiniciamos el reloj

        // Si tiene hueco para bajar, baja
        if (canMove(0, 1)) {
            m_fallingY++;
        }
        else {
            // Si no puede bajar mas, aterra en esa casilla
            landBlock();
        }
    }
}

/**
 * render
 * Dibuja en pantalla el estado calculado en update.
 */
void Game::render(GraphicManager& graphics)
{
    // Margenes visuales para la cuadricula de juego
    const int startX = 50;
    const int startY = 80;

    // 1. Dibujamos el rectangulo que bordea el tablero
    graphics.drawRectangle(
        startX - 2, startY - 2,
        CANDY_IMAGE_WIDTH * m_board->getWidth() + 4,
        CANDY_IMAGE_HEIGHT * m_board->getHeight() + 4,
        5, 150, 150, 150);

    // 2. Dibujamos todos los caramelos fijados dentro del tablero
    for (int x = 0; x < m_board->getWidth(); x++) {
        for (int y = 0; y < m_board->getHeight(); y++) {
            Candy* c = m_board->getCell(x, y);
            if (c != nullptr) {
                graphics.drawImage(
                    (*c).getResourceName(),
                    startX + (x * CANDY_IMAGE_WIDTH),
                    startY + (y * CANDY_IMAGE_HEIGHT)
                );
            }
        }
    }

    // 3. Dibujamos el bloque de 3 piezas que esta cayendo (solo las partes visibles)
    for (int i = 0; i < 3; i++) {
        if (m_fallingBlock[i] != nullptr) {
            int drawY = m_fallingY + i;
            // Solo lo dibujamos si ya ha entrado en la zona visual del tablero (Y >= 0)
            if (drawY >= 0) {
                graphics.drawImage(
                    (*m_fallingBlock[i]).getResourceName(),
                    startX + (m_fallingX * CANDY_IMAGE_WIDTH),
                    startY + (drawY * CANDY_IMAGE_HEIGHT)
                );
            }
        }
    }

    // 4. Dibujar la Interfaz de Usuario (UI) y Textos
    graphics.drawImage("img/logo_small.png", 10, 10);

    // Instrucciones
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
        "Buttons: [Q] [W] [E]  --  Exit [ESC]",
        25, 700, 20, 100, 100, 100);

    // Puntuacion
    graphics.drawText("Score: " + std::to_string(m_score), 450, 10, 70, 125, 200, 125);

    // Texto de perder
    if (m_gameOver) {
        graphics.drawText("GAME OVER", 50, 350, 80, 255, 0, 0);
    }
}

/**
 * run
 * (Metodo por defecto proporcionado, no modificar)
 */
void Game::run()
{
    const int screen_width = 750;
    const int screen_height = 750;
    const int bg_red = 255;
    const int bg_green = 255;
    const int bg_blue = 255;
    runGraphicGame(*this, screen_width, screen_height, bg_red, bg_green, bg_blue);
}

/**
 * dump
 * Guarda toda la partida (Tablero, Variables y Bloque cayendo) en un fichero txt.
 */
bool Game::dump(const std::string& output_path) const
{
    ofstream fitxer(output_path);
    if (!fitxer.is_open()) return false;

    // Primero guardamos los datos basicos del Game
    fitxer << m_score << " " << m_frames << " " << m_gameOver << "\n";

    // Guardamos la posicion de la pieza que cae
    fitxer << m_fallingX << " " << m_fallingY << "\n";

    // Guardamos los 3 colores del bloque que cae (o -1 si esta vacio)
    for (int i = 0; i < 3; i++) {
        if (m_fallingBlock[i] != nullptr) {
            fitxer << (int)(*m_fallingBlock[i]).getType() << " ";
        }
        else {
            fitxer << "-1 ";
        }
    }
    fitxer << "\n";

    // Por ultimo, pedimos al tablero que se guarde el mismo en el archivo temporal del directorio data/
    // (Y luego lo leemos para copiarlo a nuestro archivo general, o simplemente guardamos todo ahi)
    // Para no complicarlo, vamos a guardar el tablero de forma nativa aqui
    for (int y = 0; y < m_board->getHeight(); y++) {
        for (int x = 0; x < m_board->getWidth(); x++) {
            Candy* c = m_board->getCell(x, y);
            if (c == nullptr) {
                fitxer << "-1 ";
            }
            else {
                fitxer << (int)(*c).getType() << " ";
            }
        }
        fitxer << "\n";
    }

    fitxer.close();
    return true;
}

/**
 * load
 * Carga una partida guardada previamente por dump.
 */
bool Game::load(const std::string& input_path)
{
    ifstream fitxer(input_path);
    if (!fitxer.is_open()) return false;

    // Leemos los datos basicos
    fitxer >> m_score >> m_frames >> m_gameOver;
    fitxer >> m_fallingX >> m_fallingY;

    // Leemos el bloque que cae. Si habia alguno antes, lo borramos
    for (int i = 0; i < 3; i++) {
        if (m_fallingBlock[i] != nullptr) {
            delete m_fallingBlock[i];
            m_fallingBlock[i] = nullptr;
        }
        int typeVal;
        fitxer >> typeVal;
        if (typeVal != -1) {
            m_fallingBlock[i] = new Candy((CandyType)typeVal);
        }
    }

    // Leemos el tablero 
    for (int y = 0; y < m_board->getHeight(); y++) {
        for (int x = 0; x < m_board->getWidth(); x++) {
            // Vaciamos la casilla vieja si la hubiera para no tener leaks
            Candy* cOld = m_board->getCell(x, y);
            if (cOld != nullptr) {
                delete cOld;
                m_board->setCell(nullptr, x, y);
            }

            int typeValue;
            if (fitxer >> typeValue) {
                if (typeValue != -1) {
                    m_board->setCell(new Candy((CandyType)typeValue), x, y);
                }
            }
        }
    }

    fitxer.close();
    return true;
}

/**
 * operator==
 * Sobrecarga que permite verificar si dos partidas (g1 == g2) son identicas.
 */
bool Game::operator==(const Game& other) const
{
    // 1. Verificacion rapida de variables primarias
    if (m_score != other.m_score ||
        m_frames != other.m_frames ||
        m_gameOver != other.m_gameOver ||
        m_fallingX != other.m_fallingX ||
        m_fallingY != other.m_fallingY)
    {
        return false;
    }

    // 2. Comprobar que los bloques que caen sean iguales
    for (int i = 0; i < 3; i++) {
        Candy* c1 = m_fallingBlock[i];
        Candy* c2 = other.m_fallingBlock[i];

        if (c1 == nullptr && c2 != nullptr) return false;
        if (c1 != nullptr && c2 == nullptr) return false;
        if (c1 != nullptr && c2 != nullptr && (*c1).getType() != (*c2).getType()) return false;
    }

    // 3. Finalmente comprobamos la cuadricula entera del tablero
    for (int x = 0; x < m_board->getWidth(); x++) {
        for (int y = 0; y < m_board->getHeight(); y++) {
            Candy* c1 = m_board->getCell(x, y);
            Candy* c2 = other.m_board->getCell(x, y);

            if (c1 == nullptr && c2 != nullptr) return false;
            if (c1 != nullptr && c2 == nullptr) return false;
            if (c1 != nullptr && c2 != nullptr && (*c1).getType() != (*c2).getType()) return false;
        }
    }

    return true; // Son clonicas
}