#include "board.h"
#include <memory>
#include <iostream>

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
    
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        out = m_grid[x][y];
    }
        
    return out;
}

void Board::setCell(Candy* candy, int x, int y)
{
    // Implement your code here
}


int Board::getWidth() const
{
    return m_width;
}


int Board::getHeight() const
{
    return m_height;
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
