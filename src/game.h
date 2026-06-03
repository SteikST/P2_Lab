/**
 * FITXER game.h
 * AUTOR Ferran Sanchez Bargas i Sabir Allouch El Imrani
 * DATA 02/06/2026
 * VERSIO 2.0 (Segon Lliurament - Motor i Grafics)
 * Declaracio de la classe Game i les seves variables per gestionar
 * la logica del joc en temps real (60 fps), inputs i renderitzat.
 */

#ifndef GAME_H
#define GAME_H

#include "graphics.h"
#include "controller.h"
#include "board.h" //Necessitem el Board per gestionar el tauler

 /**
  * CLASS Game
  * Main game class: keep track of the game state.
  * When run_graphic_game() is called, the game loop will call
  * update() and render() every frame, in that order.
  */
class Game
{
public:
    /// Inicializa el juego, el tablero dinamico y spawnea el primer bloque.
    Game();

    /// Destruye todos los caramelos y el tablero (Evitar Memory Leaks).
    ~Game();

    /// Run the game loop
    void run();

    /**
     * Update the game state. Called every frame when run().
     * This part is not expected to do any rendering.
     * @param controller the Controller to use for input handling.
     */
    void update(const Controller& controller);

    /**
     * Draw the next frame. Called once per frame, after update().
     * @param graphics the GraphicManager to use for rendering.
     */
    void render(GraphicManager& graphics);

    /**
     * Serialize and write the current game state to a file.
     * @param output_path path where to save the game state.
     */
    bool dump(const std::string& output_path) const;

    /**
     * Load a serialized game state from a file created with dump().
     * @param input_path path from which to load the game state.
     */
    bool load(const std::string& input_path);

    /// @return true if this game is equal to the other game
    bool operator==(const Game& other) const;

private:

    // --- VARIABLES DE ESTADO DEL JUEGO ---
    Board* m_board;               // Puntero al tablero dinamico
    int m_score;                  // Puntuacion del jugador
    int m_frames;                 // Contador para calcular la caida por gravedad (1 pos/seg)
    bool m_gameOver;              // Estado de fin de partida

    // --- VARIABLES DEL BLOQUE QUE CAE ---
    Candy* m_fallingBlock[3];     // Un bloque consta de 3 caramelos
    int m_fallingX;               // Posicion X del bloque (columna)
    int m_fallingY;               // Posicion Y del primer caramelo del bloque (puede ser negativo)

    // --- VARIABLES PARA EL TECLADO (DEBOUNCE) ---
    // Nos sirven para evitar que si dejas pulsada una tecla, se mueva a la velocidad de la luz
    bool m_leftLastFrame;
    bool m_rightLastFrame;
    bool m_rotLastFrame;
    bool m_saveLastFrame;

    // --- METODOS PRIVADOS AUXILIARES ---
    /**
     * Genera un nuevo bloque de 3 piezas en la columna 6, por encima del tablero.
     */
    void spawnBlock();

    /**
     * Verifica si el bloque actual se puede mover en las direcciones dadas sin chocar.
     */
    bool canMove(int dx, int dy) const;

    /**
     * Fija el bloque en el tablero actual e inicia las explosiones en cadena.
     */
    void landBlock();
};
#endif