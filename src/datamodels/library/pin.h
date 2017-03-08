#ifndef PIN_H
#define PIN_H

#include "src/datamodels/serializable.h"

enum class PinType
{
    Input,
    Output,
};

/**
 * @brief The Pin class
 * Represents a library element pin.
 */
class Pin : public Serializable
{
protected:
    QString id;
    int x, y;
    PinType type;

public:
    Pin(QString id, int x, int y, PinType type);

    void setId (QString id);
    void setX (int x);
    void setY (int y);
    void setType (PinType type);

    QString getId() {return id;}
    int getX() {return x;}
    int getY() {return y;}
    PinType getType() {return type;}
};

#endif // PIN_H