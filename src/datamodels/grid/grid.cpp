#include "grid.h"

Grid::Grid(int initialLevel)
{
    setInitialLevel(initialLevel);
}

Grid::~Grid()
{

}

void Grid::setInitialLevel(int initialLevel)
{
    if (initialLevel < 0)
        throw IllegalArgumentException("Client initial level cannot be negative");

    this->initialLevel = initialLevel;
}
