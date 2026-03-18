#include "board.h"
#include <memory>
#include <iostream>

Board::Board(int width, int height)
{
    //Creacio main
    //Evitar FF
}

Board::~Board()
{
    // Implement your code here
}


Candy* Board::getCell(int x, int y) const
{
    // Implement your code here
    return nullptr;
}

void Board::setCell(Candy* candy, int x, int y)
{
    // Implement your code here
}


int Board::getWidth() const
{
    // Implement your code here
    return -1;
}


int Board::getHeight() const
{
    // Implement your code here
    return -1;
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
