#include "outerconnectionsnumbercomparator.h"

OuterConnectionsNumberComparator::OuterConnectionsNumberComparator(QList<SchemeVertex*> &list) :
    list(list)
{

}

bool OuterConnectionsNumberComparator::operator ()(SchemeVertex* v1, SchemeVertex* v2) const
{
    int firstConnectionsNumber = 0;
    int secondConnectionsNumber = 0;

    for(std::pair<SchemeVertex*, WireType>& connectedElement: v1->getConnectedElements())
    {
        if(!list.contains(connectedElement.first))
            firstConnectionsNumber ++;
    }

    for(std::pair<SchemeVertex*, WireType>& connectedElement: v2->getConnectedElements())
    {
        if(!list.contains(connectedElement.first))
            secondConnectionsNumber ++;
    }

    return (firstConnectionsNumber < secondConnectionsNumber);
}
