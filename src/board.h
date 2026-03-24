/**
 * FITXER board.h
 * AUTOR Ferran Sánchez Bargas i Sabir Allouch El Imrani
 * DATA 24/03/2026
 * VERSIO 1.0
 * Declaració de la classe Board i les seves constants relacionades per a la
 * gestió del tauler de joc.
 */

#ifndef BOARD_H
#define BOARD_H

#include "candy.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

 /// Default board dimensions
const int DEFAULT_BOARD_WIDTH = 10;
const int DEFAULT_BOARD_HEIGHT = 10;

/// Number of candy pieces of the same type needed to explode a horizontal, vertical or diagonal line
const int SHORTEST_EXPLOSION_LINE = 3;

/**
 * CLASS Board
 * Square game board state with r/w access to individual cells.
 * Gestiona les dimensions del joc i la quadrícula de caramels.
 */
class Board
{
public:
    /**
     * Board
     * Initialize a board of the given dimensions.
     * @param width: amplada del tauler (per defecte DEFAULT_BOARD_WIDTH)
     * @param height: alçada del tauler (per defecte DEFAULT_BOARD_HEIGHT)
     */
    Board(int width = DEFAULT_BOARD_WIDTH, int height = DEFAULT_BOARD_HEIGHT);

    /**
     * ~Board
     * Free all resources managed by the board
     */
    ~Board();

    /**
     * shouldExplode
     * Determine whether the candy at the given coordinates should explode
     * given the current board state, i.e., is part of a (h/v/d) line of
     * SHORTEST_EXPLOSION_LINE or more candies of the same type.
     * @param x: 0-indexed, left to right, x coordinate of the cell
     * @param y: 0-indexed, top to bottom, y coordinate of the cell
     * @return true if the candy at the given coordinates should explode, false otherwise.
     */
    bool shouldExplode(int x, int y) const;

    /**
     * explodeAndDrop
     * Explode all candies that should explode (lines of candies)
     * and then let candies above drop down to fill the gaps.
     * After dropping, repeat until no more candies can explode.
     * @return a (potentially empty) vector of all exploded candies.
     */
    std::vector<Candy*> explodeAndDrop();

    /**
     * dump
     * Save a serialized representation of the board to a file, which can be later loaded
     * with Board::load.
     * @param output_path: path to the output file where the representation is to be saved.
     * @return true if the dump was successful, false otherwise.
     */
    bool dump(const std::string& output_path) const;

    /**
     * load
     * Load a serialized representation of the board from a file previously saved.
     * Any existing board state is discarded.
     * @param input_path: path to the input file from which the representation is to be loaded.
     * @return true if the load was successful, false otherwise.
     */
    bool load(const std::string& input_path);

    /**
     * getCell
     * Get a reference to the candy piece at the given coordinates if there is one.
     * @param x: 0-indexed, left to right, x coordinate of the cell
     * @param y: 0-indexed, top to bottom, y coordinate of the cell
     * @return a pointer to the candy at the given coordinates,
     * if the coordinates are valid and the cell is not empty; nullptr otherwise.
     */
    Candy* getCell(int x, int y) const;

    /**
     * setCell
     * Set the cell at the given coordinates to the given candy.
     * @param candy: the candy to place in the cell.
     * @param x: 0-indexed, left to right, x coordinate of the cell.
     * @param y: 0-indexed, top to bottom, y coordinate of the cell.
     */
    void setCell(Candy* candy, int x, int y);

    /**
     * getWidth
     * Get the board width
     * @return int representing the board width
     */
    int getWidth() const;

    /**
     * getHeight
     * Get the board height
     * @return int representing the board height
     */
    int getHeight() const;

private:
    int m_width;
    int m_height;

    // Vector dentro de un vector, que apunta a candy, nombre asignado m_grid. 
    // Cada cuadricula contiene un caramelo o nada (nullptr)
    std::vector< std::vector<Candy*> > m_grid;

    /**
     * countInDirection
     * Mètode auxiliar per comptar quants caramels del mateix tipus hi ha en una direcció.
     * @param startX: posició x inicial
     * @param startY: posició y inicial
     * @param dx: direcció en l'eix x
     * @param dy: direcció en l'eix y
     * @param type: Tipus de caramel a cercar
     * @return Nombre de caramels consecutius
     */
    int countInDirection(int startX, int startY, int dx, int dy, CandyType type) const;

    /// Students can add as many protected methods and attributes as needed.
};

#endif