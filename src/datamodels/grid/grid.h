#ifndef GRID_H
#define GRID_H

#include <QList>

#include "cell.h"
#include "routedwireindex.h"

/**
 * @brief The Grid class
 */
class Grid: public Serializable
{
protected:
    QList<QList<Cell>> cells;
    QList <RoutedWireIndex> routedWires;

public:
    Grid() {}
    virtual ~Grid() {}

    QList<QList<Cell>>& getCells() { return cells; }
    QList <RoutedWireIndex>& getRoutedWires() { return routedWires; }

    bool operator ==(const Grid& other);
};

#endif // GRID_H
