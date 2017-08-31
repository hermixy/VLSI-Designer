#ifndef LEEROUTING_H
#define LEEROUTING_H

#include <QList>

#include "algorithms/routing/routingalgorithm.h"
#include "util/misc/schemeutils.h"
#include "util/misc/wireutils.h"
#include "wiredistancecomparator.h"
#include "pointdistancecomparator.h"

struct CellInfo
{
    qint64 value;
    bool branched;
};

/**
 * @brief The LeeRouting class
 * Implements the Lee path connection algorithm.
 */
class LeeRouting : public RoutingAlgorithm
{
public:
    LeeRouting(Grid* grid, Scheme* scheme, PrimaryPlacementAlgorithm* algorithm, int maxExtensionAttempts);

    /**
     * @brief execute
     * Executes the algorithm with the given parameters.
     * @throw Exception
     * @return
     */
    Grid* execute();

private:
    QList<WireData*> innerWires, outerWires;
    QList<QList<CellInfo>> matrix;

    int gridHeight, gridWidth;
    qint64 currentValue;
    int extensionAttempts;

    bool finished;
    bool noMoreWays;

    QPoint startCoord, finishCoord;
    QPoint startPinCoord, finishPinCoord;
    bool startBranched;
    bool finishBranched;

    void clear();

    void initWires();
    bool isWireRouted(Wire& wire);

    void routeWire(WireData *data);
    void initMatrix();
    std::pair<QPoint, bool> getNearbyAvailableCoord(QPoint pinCoord);

    void pushWave();
    bool tryRoute(QPoint from, QPoint to);

    bool tryExtend();

    void pushReverseWave();
};

#endif // LEEROUTING_H
