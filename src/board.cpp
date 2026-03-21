#include "board.h"
#include <memory>
#include <iostream>

using namespace std;

Board::Board(int width, int height)
{
    m_width = width;
    m_height = height;

    //.assign sirve para rellenar m_grid, estamos metiendo en cada punto 
    //de la fila (m_width) una columna de largo m_height con casillas vacias (nullptr)
    m_grid.assign(m_width, std::vector<Candy*>(m_height, nullptr));
}

Board::~Board()
{
    //Aun no nos hace falta implementarlo
    // Implement your code here
}

//Candy* significa que devuelve un puntero de candy
//m_grid[x][y] contiene un puntero candy

Candy* Board::getCell(int x, int y) const
{
    Candy* out = nullptr;
    
    //Verificamos que la posicion sea valida
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        out = m_grid[x][y];
    }
        
    return out;
}

void Board::setCell(Candy* candy, int x, int y)
{
    //Verificamos que la poscion sea valida
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        m_grid[x][y] = candy;
    }
}


int Board::getWidth() const
{
    return m_width;
}


int Board::getHeight() const
{
    return m_height;
}

//countInDirecton metodo auxiliar propio creado como ayuda para shouldExplode
//CanyType es una clase enumerada (una lista para darle nombres a los colores de los caramelos
//situada en candy.h. La clase asigna numeros a las  palabras RED=0, BLUE=1...
//dx y dy definen la direccion en la que nos moveremos: dx=1, dy = 0 derecha, etc..
int Board::countInDirection(int startX, int startY, int dx, int dy, CandyType type) const
{
    
    //Cuenta la cantidad de caramelos iguales despues del inicial en la
    //direccion indicada por dx y dy
    int comptador = 0;

    //Nos movemos una posicion a partir de la posicion inicial
    int currentX = startX + dx;
    int currentY = startY + dy;

    //Miramos el tipo de caramela en la posicion actual
    Candy* c = getCell(currentX, currentY);

    //Verificamos si el caramelo es el mismo que el de la posicion inicial
    while (c != nullptr && (*c).getType() == type)
    {
        comptador++;

        //Nos movemos una posicion a partir de la posicion actual
        currentX += dx;
        currentY += dy;

        //Miramos el tipo de caramelo en la posicion actual
        c = getCell(currentX, currentY);
    }

    return comptador;
}

bool Board::shouldExplode(int x, int y) const
{
    // Implement your code here
    return false;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    // Implement your code here
    return {};
}

bool Board::dump(const std::string& output_path) const
{
    // Implement your code here
    return false;
}

bool Board::load(const std::string& input_path)
{
    // Implement your code here
    return false;
}
