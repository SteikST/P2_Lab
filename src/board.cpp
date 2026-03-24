/**
 * FITXER board.cpp
 * AUTOR Ferran Sánchez Bargas i Sabir Allouch El Imrani
 * DATA 24/03/2026
 * VERSIO 1.0
 * Implementació de la classe Board per gestionar el tauler de joc.
 */

#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>

using namespace std;

/**
 * Board
 * Constructor de la classe Board. Inicialitza les dimensions i el grid.
 * @param width: amplada del tauler
 * @param height: alçada del tauler
 */
Board::Board(int width, int height) : m_width(width), m_height(height)
{
    // .assign sirve para rellenar m_grid, estamos metiendo en cada punto 
    // de la fila (m_width) una columna de largo m_height con casillas vacias (nullptr)
    m_grid.assign(m_width, vector<Candy*>(m_height, nullptr));
}

/**
 * ~Board
 * Destructor de la classe Board.
 */
Board::~Board()
{
    // Aun no hace falta hacerlo
}

/**
 * getCell
 * Retorna el punter al caramel de la posició donada.
 * @param x: posició x en el tauler
 * @param y: posició y en el tauler
 * @return Punter al caramel, o nullptr si no hi ha caramel o la posició no és vàlida
 */
Candy* Board::getCell(int x, int y) const
{
    Candy* out = nullptr;

    // Verificamos que la posicion sea valida
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        out = m_grid[x][y];
    }

    return out;
}

/**
 * setCell
 * Assigna un caramel a una posició específica del tauler.
 * @param candy: Punter al caramel que volem col·locar
 * @param x: posició x en el tauler
 * @param y: posició y en el tauler
 */
void Board::setCell(Candy* candy, int x, int y)
{
    // Verificamos que la poscion sea valida
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        m_grid[x][y] = candy;
    }
}

/**
 * getWidth
 * Retorna l'amplada del tauler.
 * @return Amplada del tauler
 */
int Board::getWidth() const
{
    return m_width;
}

/**
 * getHeight
 * Retorna l'alçada del tauler.
 * @return Alçada del tauler
 */
int Board::getHeight() const
{
    return m_height;
}

/**
 * countInDirection
 * Mètode auxiliar per comptar quants caramels del mateix tipus hi ha en una direcció.
 * @param startX: posició x inicial
 * @param startY: posició y inicial
 * @param dx: direcció en l'eix x (-1, 0, 1)
 * @param dy: direcció en l'eix y (-1, 0, 1)
 * @param type: Tipus de caramel a cercar
 * @return Nombre de caramels consecutius del mateix tipus
 */
int Board::countInDirection(int startX, int startY, int dx, int dy, CandyType type) const
{
    // Cuenta la cantidad de caramelos iguales despues del inicial en la
    // direccion indicada por dx y dy
    int comptador = 0;

    // Nos movemos una posicion a partir de la posicion inicial
    int currentX = startX + dx;
    int currentY = startY + dy;

    // Miramos el tipo de caramela en la posicion actual
    Candy* c = getCell(currentX, currentY);

    // Verificamos si el caramelo es el mismo que el de la posicion inicial
    while (c != nullptr && (*c).getType() == type)
    {
        comptador++;

        // Nos movemos una posicion a partir de la posicion actual
        currentX += dx;
        currentY += dy;

        // Miramos el tipo de caramelo en la posicion actual
        c = getCell(currentX, currentY);
    }

    return comptador;
}

/**
 * shouldExplode
 * Verifica si el caramel en una posició ha d'explotar (forma línia de 3 o més).
 * @param x: posició x a verificar
 * @param y: posició y a verificar
 * @return true si ha d'explotar, false en cas contrari
 */
bool Board::shouldExplode(int x, int y) const
{
    // Por defecto no explota
    bool out = false;

    // Obtenemos el tipo de caramelo por referencia de la casilla central
    Candy* centerCandy = getCell(x, y);

    // Si en la casilla central no hay caramelo, se acaba, sino seguimos
    if (centerCandy != nullptr)
    {
        // Conseguimos el tipo de caramelo, para el metodo countInDirection
        CandyType type = (*centerCandy).getType();

        // Calculamos las cuatro líneas posibles, el +1 es para contar el caramelo central tambien
        int horizontal = countInDirection(x, y, -1, 0, type) + countInDirection(x, y, 1, 0, type) + 1;
        int vertical = countInDirection(x, y, 0, -1, type) + countInDirection(x, y, 0, 1, type) + 1;
        int diagDownUp = countInDirection(x, y, -1, -1, type) + countInDirection(x, y, 1, 1, type) + 1;
        int diagUpDown = countInDirection(x, y, -1, 1, type) + countInDirection(x, y, 1, -1, type) + 1;

        // Si alguna de las líneas llega al mínimo para explotar, ponemos la salida en true
        if (horizontal >= SHORTEST_EXPLOSION_LINE || vertical >= SHORTEST_EXPLOSION_LINE ||
            diagDownUp >= SHORTEST_EXPLOSION_LINE || diagUpDown >= SHORTEST_EXPLOSION_LINE)
        {
            out = true;
        }
    }

    return out;
}

/**
 * explodeAndDrop
 * Cerca caramels que han d'explotar, els elimina i fa caure els de dalt.
 * @return Vector amb els punters als caramels que han explotat
 */
vector<Candy*> Board::explodeAndDrop()
{
    // Declaracion del vector que retornamos con la cantidad de caramelos explotados
    vector<Candy*> explodedTotal;

    // Detecta si ha habido una explosion
    bool hadExplosions;

    do
    {
        // Al inicio no hay ninguna explosion = false
        hadExplosions = false;

        // Declaramos un vector dentro de un vector que contiene valores de true (explota)
        // o false (no explota) con el tamaño del tablero que es por defecto 10x10
        vector< vector<bool> > toExplode(m_width, vector<bool>(m_height, false));

        // Este for identifica las casillas que deben explotar pasando por todas ellas
        for (int x = 0; x < m_width; x++)
        {
            for (int y = 0; y < m_height; y++)
            {
                // Usamos el metodo shouldExplode, devuelve true si hay 3
                // caramelos o mas del mismo tipo y por lo tanto deben explotar
                if (shouldExplode(x, y))
                {
                    // Ponemos en true la casilla donde el caramelo explota
                    // y true hadExplosions porque ha habido una explosion
                    toExplode[x][y] = true;
                    hadExplosions = true;
                }
            }
        }

        // Si ha habido al menos una explosion se ejecuta
        if (hadExplosions)
        {
            // Este for guarda la cantidad de caramelos que han explotado y vacia sus casillas
            for (int x = 0; x < m_width; x++)
            {
                for (int y = 0; y < m_height; y++)
                {
                    // Si explota se ejecuta
                    if (toExplode[x][y])
                    {
                        // Guarda la referencia del caramelo explotado
                        Candy* c = getCell(x, y);

                        // Si la casilla no esta vacia, guarda el caramelo en
                        // explodedTotal, ampliando el vector y añadiendo su refencia
                        if (c != nullptr)
                        {
                            explodedTotal.push_back(c);
                            // Eliminamos el caramelo que debe explotar
                            setCell(nullptr, x, y);
                        }
                    }
                }
            }

            for (int x = 0; x < m_width; x++)
            {
                // 'writeY' es el "hueco destino". Empezamos asumiendo que la posición