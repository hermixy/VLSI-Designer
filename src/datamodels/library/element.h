#ifndef ELEMENT_H
#define ELEMENT_H

#include <QList>
#include "pin.h"

/**
 * @brief The Element class
 * Represents a library element.
 * Note that an instance of this class will delete all contained pins in destructor.
 */
class Element : public Serializable
{
protected:
    QString id;
    QString name, model;
    int height, width;
    QList <Pin*> pins;

public:
    Element(QString id, int height, int width);
    ~Element();

    void setId (QString id);
    void setName (QString name);
    void setModel (QString model);
    void setHeight (int height);
    void setWidth (int width);

    QString getId() {return this->id;}
    QString getName() {return this->name;}
    QString getModel() {return this->model;}
    int getHeight() {return this->height;}
    int getWidth() {return this->width;}
    QList <Pin*>& getPins() {return this->pins;}
};

#endif // ELEMENT_H