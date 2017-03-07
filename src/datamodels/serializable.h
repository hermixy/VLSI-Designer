#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <QObject>
#include <QString>

#include "src/exception/illegalargumentexception.h"

/**
 * @brief The Serializable class
 * Base class for all models which can be serialized or deserialized.
 */
class Serializable: public QObject
{
public:
    Serializable();
};

#endif // SERIALIZABLE_H
