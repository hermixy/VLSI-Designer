#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <QString>

#include "exception/illegalargumentexception.h"

/**
 * @brief The Serializable class
 * Base class for all models which can be serialized or deserialized.
 */
class Serializable
{
public:
    Serializable();
    virtual ~Serializable() = 0;
};

#endif // SERIALIZABLE_H
