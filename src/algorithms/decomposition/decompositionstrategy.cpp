#include "decompositionstrategy.h"

DecompositionStrategy::DecompositionStrategy()
{
    algorithms.append(QObject::tr("Serial"));
}

DecompositionAlgorithm* DecompositionStrategy::createAlgorithm(int index, Scheme* scheme, int number)
{
    switch(index)
    {
    case 0:
        return new SerialDecomposition(scheme, number);
    default:
        throw IllegalArgumentException(QObject::tr("Cannot instantiate an algorithm with type %1.").arg(index));
    }
}
