#include "jsonserializer.h"

JsonSerializer::JsonSerializer()
{

}

QByteArray JsonSerializer::serialize(Serializable* s)
{
    const std::type_info& info = typeid(*s);
    if (info == typeid(Library))
        return serializeLibrary(static_cast<Library*>(s));
    else if (info == typeid(Scheme))
        return serializeScheme(static_cast<Scheme*>(s));
    else
        throw IllegalArgumentException(QString("This type is not supported: %1").arg(typeid(*s).name()));
}

QByteArray JsonSerializer::serializeLibrary(Library* l)
{
    QJsonObject json;

    json["id"] = l->getId();
    json["version"] = l->getVersion();
    json["name"] = l->getName();

    QJsonArray elements;
    for(Element* el: l->getElements())
        elements.append(serializeElement(el));
    json["elements"] = elements;

    QJsonObject res;
    res["library"] = json;

    QJsonDocument doc(res);
    return doc.toJson();
}

QJsonObject JsonSerializer::serializeElement(Element* el)
{
    QJsonObject json;

    json["id"] = el->getId();
    json["name"] = el->getName();
    json["model"] = el->getModel();
    json["height"] = el->getHeight();
    json["width"] = el->getWidth();

    QJsonArray pins;
    for(Pin* pin: el->getPins())
        pins.append(serializePin(pin));

    json["pins"] = pins;

    return json;
}

QJsonObject JsonSerializer::serializePin(Pin* p)
{
    QJsonObject json;

    json["id"] = p->getId();
    json["x"] = p->getX();
    json["y"] = p->getY();
    QString type = pinTypeMap.key(p->getType());
    json["type"] = type;

    return json;
}

QByteArray JsonSerializer::serializeScheme(Scheme* s)
{
    QJsonObject json;

    QJsonArray elements, wires;

    for(SchemeElement* el: s->getElements())
        elements.append(serializeSchemeElement(el));

    for(Wire* w: s->getWires())
        wires.append(serializeWire(w));

    json["elements"] = elements;
    json["wires"] = wires;

    QJsonObject res;
    res["scheme"] = json;

    QJsonDocument doc(res);
    return doc.toJson();
}

QJsonObject JsonSerializer::serializeSchemeElement(SchemeElement* el)
{
    QJsonObject json;

    json["library-id"] = el->getLibraryId();
    json["element-id"] = el->getElementId();
    json["index"] = QString::number(el->getIndex());

    return json;
}

QJsonObject JsonSerializer::serializeWire(Wire* w)
{
    QJsonObject json;

    json["src-index"] = QString::number(w->getSrcIndex());
    json["src-pin-id"] = w->getSrcPinId();
    json["dest-index"] = QString::number(w->getDestIndex());
    json["dest-pin-id"] = w->getDestPinId();

    QString type = wireTypeMap.key(w->getType());
    json["type"] = type;
    json["index"] = QString::number(w->getIndex());

    return json;
}

Serializable* JsonSerializer::deserialize(QByteArray jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if(!doc.isObject())
        throw IllegalArgumentException ("Invalid JSON");

    QJsonObject obj = doc.object();
    QString key = obj.keys()[0];

    if (key == "library")
        return deserializeLibrary(obj.value("library").toObject());
    else if (key == "scheme")
        return deserializeScheme(obj.value("scheme").toObject());
    else if (key == "grid")
        return deserializeGrid(obj.value("grid").toObject());
    else if (key == "architecture")
        return deserializeArchitecture(obj.value("architecture").toObject());
    else
        throw IllegalArgumentException ("The contained JSON object is not supported or cannot be deserialized directly");
}

Library* JsonSerializer::deserializeLibrary (QJsonObject obj)
{
    QString id = obj.value("id").toString();
    double version = obj.value("version").toDouble(-1.0);

    Library* library = new Library(id, version);
    library->setName(obj.value("name").toString());

    QJsonArray elements = obj.value("elements").toArray();
    for(QJsonValue val: elements)
        library->getElements().append(deserializeElement(val.toObject()));

    return library;
}

Element* JsonSerializer::deserializeElement (QJsonObject obj)
{
    QString id = obj.value("id").toString();

    int height = obj.value("height").toInt(-1);
    int width = obj.value("width").toInt(-1);

    Element* element = new Element(id, height, width);
    element->setName(obj.value("name").toString());
    element->setModel(obj.value("model").toString());

    QJsonArray pins = obj.value("pins").toArray();
    for (QJsonValue val: pins)
        element->getPins().append(deserializePin(val.toObject()));

    return element;
}

Pin* JsonSerializer::deserializePin (QJsonObject obj)
{
    QString id = obj.value("id").toString();

    int x = obj.value("x").toInt(-1);
    int y = obj.value("y").toInt(-1);

    QString t = obj.value("type").toString();
    PinType type;
    if (pinTypeMap.contains(t))
        type = pinTypeMap[t];
    else
        throw IllegalArgumentException("Invalid pin type specified");

    return new Pin (id, x, y, type);
}

Scheme* JsonSerializer::deserializeScheme (QJsonObject obj)
{
    Scheme* scheme = new Scheme();

    QJsonArray elements = obj.value("elements").toArray();
    for (QJsonValue val: elements)
        scheme->getElements().append(deserializeSchemeElement(val.toObject()));

    QJsonArray wires = obj.value("wires").toArray();
    for (QJsonValue val: wires)
        scheme->getWires().append(deserializeWire(val.toObject()));

    return scheme;
}

SchemeElement* JsonSerializer::deserializeSchemeElement (QJsonObject obj)
{
    QString libraryId = obj.value("library-id").toString();
    QString elementId = obj.value("element-id").toString();

    bool ok;
    qint64 index = obj.value("index").toString().toLongLong(&ok);
    if (!ok) index = -1;

    return new SchemeElement(libraryId, elementId, index);
}

Wire* JsonSerializer::deserializeWire (QJsonObject obj)
{
    bool ok;
    qint64 srcIndex = obj.value("src-index").toString().toLongLong(&ok);
    if (!ok) srcIndex = -1;
    QString srcPinId = obj.value("src-pin-id").toString();

    qint64 destIndex = obj.value("dest-index").toString().toLongLong(&ok);
    if (!ok) destIndex = -1;
    QString destPinId = obj.value("dest-pin-id").toString();

    QString t = obj.value("type").toString();
    WireType type;
    if (wireTypeMap.contains(t))
        type = wireTypeMap[t];
    else
        throw IllegalArgumentException("Invalid wire type specified");

    qint64 index = obj.value("index").toString().toLongLong(&ok);
    if (!ok) index = -1;

    return new Wire (srcIndex, srcPinId, destIndex, destPinId, type, index);
}

Grid* JsonSerializer::deserializeGrid (QJsonObject obj)
{
    int initialLevel = obj.value("initial-level").toInt();
    Grid* grid = new Grid(initialLevel);

    QJsonArray cells = obj.value("cells").toArray();
    for (QJsonValue val: cells)
    {
        QList<Cell*> rowList;
        QJsonArray row = val.toArray();
        for(QJsonValue cell: row)
        {
            rowList.append(deserializeCell(cell.toObject()));
        }
        grid->getCells().append(rowList);
    }

    QJsonArray routedWires = obj.value("routed-wires").toArray();
    for(QJsonValue val: routedWires)
    {
        bool ok;
        qint64 index =val.toString().toLongLong(&ok);
        if (!ok)
            throw IllegalArgumentException("Invalid routed wire index");

        grid->getRoutedWires().append(index);
    }

    return grid;
}

Cell* JsonSerializer::deserializeCell (QJsonObject obj)
{
    QString t = obj.value("type").toString();
    CellType type;
    if (cellTypeMap.contains(t))
        type = cellTypeMap[t];
    else
        throw IllegalArgumentException("Invalid cell type specified");

    bool ok;
    qint64 index = obj.value("index").toString().toLongLong(&ok);
    if (!ok) index = -1;

    QString pinId = obj.value("pin-id").toString();

    return new Cell(type, index, pinId);
}

Architecture* JsonSerializer::deserializeArchitecture(QJsonObject obj)
{
    QString t = obj.value("distribution-type").toString();
    DistributionType type;
    if (distributionTypeMap.contains(t))
        type = distributionTypeMap[t];
    else
        throw IllegalArgumentException("Invalid distribution type specified");

    Architecture* architecture = new Architecture(type);

    QJsonArray model = obj.value("model").toArray();
    for (QJsonValue val: model)
        architecture->getModel().append(val.toInt());

    return architecture;
}
