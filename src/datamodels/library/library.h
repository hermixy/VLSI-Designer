#ifndef LIBRARY_H
#define LIBRARY_H

#include <QList>
#include "element.h"

/**
 * @brief The Library class
 * Represents a library of elements.
 * Note that an instance of this class will delete all contained elements in destructor.
 */
class Library: public Serializable
{
protected:
    QString id;
    double version;
    QString name;
    QList<Element*> elements;

public:
    Library(QString id, double version);
    ~Library();

    void setId(QString id);
    void setVersion(double version);
    void setName (QString name);

    QString getId() {return this->id;}
    double getVersion() {return this->version;}
    QString getName() {return this->name;}
    QList<Element*>& getElements() {return this->elements;}
};

#endif // LIBRARY_H