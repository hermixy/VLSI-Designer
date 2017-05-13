#include "generator.h"

Generator::Generator(GeneratorParameters param):
    param(param),
    mt{static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())},
    nodeCapacityDistribution(param.getNodeCapacityMean(), param.getNodeCapacitySigma()),
    branchingDistribution(param.getBranchingMean(), param.getBranchingSigma()),
    libraryRandom(0, param.getLibraries().size() - 1)
{
    stopped = true;
    mt.discard(numbersToDiscard);
}

Generator::~Generator()
{
    for(Library* l: param.getLibraries())
        delete l;
}

Scheme Generator::generate()
{
    stopped = false;

    generateElements();
    generateWires();

    Scheme scheme;

    for(NodeElement el: elements)
        scheme.getElements().append(el.getElement());

    scheme.getWires().append(wires);

    stopped = true;

    return scheme;
}

void Generator::generateElements()
{
    elements.clear();
    groupedElements.clear();
    groupedElements.append(QList<NodeElement>());

    currentElementIndex = 0;

    int capacity = 0;
    int elapsedElements = param.getElementsNumber();
    int currentNodeNumber = 1;

    while((capacity = pow(2, getTruncatedDistributedValue(nodeCapacityDistribution, param.getNodeCapacityLeftLimit(), param.getNodeCapacityRightLimit())))
          <= elapsedElements)
    {
        SchemeElement element = getRandomElement();

        groupedElements.append(QList<NodeElement>());

        for(int i=0; i<capacity; i++)
        {
            SchemeElement el (element);

            if (i > 0)
            {
                el.setIndex(currentElementIndex);
                currentElementIndex ++;
            }

            NodeElement element(el, currentNodeNumber);

            elements.append(element);
            groupedElements[groupedElements.size() - 1].append(element);
        }

        currentNodeNumber ++;

        elapsedElements -= capacity;
    }

    for(int i=0; i<elapsedElements; i++)
    {
        NodeElement element = NodeElement(getRandomElement(), freeNodeElementIndex);

        elements.append(element);
        groupedElements[0].append(element);
    }
}

int Generator::getTruncatedDistributedValue(std::normal_distribution<>& dist, int leftRange, int rightRange)
{
    while(true)
    {
        int value = round(dist(mt));
        if (value >= leftRange && value <= rightRange)
            return value;
    }
}

SchemeElement Generator::getRandomElement()
{
    Library* library = param.getLibraries()[libraryRandom(mt)];

    std::uniform_int_distribution<int> libraryElementRandom(0, library->getElements().size() - 1);
    LibraryElement element = library->getElements()[libraryElementRandom(mt)];

    SchemeElement res(library->getId(), element.getId(), currentElementIndex);
    currentElementIndex ++;

    return res;
}

void Generator::generateWires()
{
    wires.clear();
    currentWireIndex = 0;

    checkBranching();

    for(int i=0; i<elements.size(); i++)
    {
        LibraryElement el = getCorrespondingElement(elements[i].getElement());

        for(Pin p: el.getPins())
        {
            if(p.getType() == PinType::Output)
            {
                generateWiresForOutput(elements[i], p);
            }
        }
    }
}

void Generator::checkBranching()
{
    qint64 inputPins = countAllInputPins();

    if(inputPins < param.getBranchingRightLimit())
    {
        int rightLimit = inputPins;
        int leftLimit = param.getBranchingLeftLimit();
        int mean = param.getBranchingMean();

        if(leftLimit > rightLimit)
            leftLimit = rightLimit;

        if(mean < leftLimit || mean > rightLimit)
            mean = (leftLimit + rightLimit) / 2;

        param.setBranching(mean, param.getBranchingSigma(), leftLimit, rightLimit);
    }
    //TODO: log this
}

qint64 Generator::countAllInputPins()
{
    qint64 res = 0;
    for(NodeElement element: elements)
    {
        LibraryElement el = getCorrespondingElement(element.getElement());
        for(Pin p: el.getPins())
            if(p.getType() == PinType::Input)
                res ++;
    }

    return res;
}

LibraryElement Generator::getCorrespondingElement(SchemeElement element)
{
    for(Library* l: param.getLibraries())
    {
        if(l->getId() == element.getLibraryId())
        {
            for(LibraryElement el: l->getElements())
            {
                if (el.getId() == element.getElementId())
                    return el;
            }
        }
    }

    throw Exception("Corresponding library element cannot be found.");
}

void Generator::generateWiresForOutput(NodeElement element, Pin p)
{
    std::uniform_real_distribution<double> wireRandom(0, 1);

    int branching = branchingDistribution(mt);

    for(int i=0; i<branching; i++)
    {
        double chance = wireRandom(mt);
        if(element.getNodeNumber() == freeNodeElementIndex || chance <= chanceForOuterWire)
        {
            generateOuterWire(element, p);
            continue;
        }

        if (!generateInnerWire(element, p, branching))
            generateOuterWire(element, p);
    }
}

void Generator::generateOuterWire(NodeElement element, Pin p)
{
    std::pair<NodeElement, Pin> pair = getRandomPin();

    while(isWireExist(element, p, pair.first, pair.second))
      pair = getRandomPin();

    wires.append(buildWire(element, p, pair.first, pair.second, WireType::Outer));
}

bool Generator::generateInnerWire(NodeElement element, Pin p, int attempts)
{
    std::pair<NodeElement, Pin> pair(getRandomPin(element.getNodeNumber()));

    for(int i=0; i<attempts; i++)
    {
        if(isWireExist(element, p, pair.first, pair.second))
        {
            pair = getRandomPin(element.getNodeNumber());
        }
        else break;
    }

    if(isWireExist(element, p, pair.first, pair.second))
        return false;

    wires.append(buildWire(element, p, pair.first, pair.second, WireType::Inner));

    return true;
}

std::pair<NodeElement, Pin> Generator::getRandomPin(int node)
{
    QList<NodeElement> chosenElements;

    if(node == freeNodeElementIndex)
        chosenElements = elements;
    else
        chosenElements = groupedElements.at(node);

    std::uniform_int_distribution<int> elementRandom(0, chosenElements.size() - 1);

    while(true)
    {
        NodeElement element(chosenElements[elementRandom(mt)]);

        LibraryElement libElement(getCorrespondingElement(element.getElement()));

        std::uniform_int_distribution<int> pinRandom(0, libElement.getPins().size() - 1);
        Pin pin(libElement.getPins()[pinRandom(mt)]);

        if(pin.getType() == PinType::Input)
            return std::make_pair(element, pin);
    }
}

bool Generator::isWireExist(NodeElement sourceElement, Pin sourcePin, NodeElement destElement, Pin destPin)
{
    for(Wire w: wires)
    {
        if(w.getSrcIndex() == sourceElement.getElement().getIndex() && w.getSrcPinId() == sourcePin.getId()
           && w.getDestIndex() == destElement.getElement().getIndex() && w.getDestPinId() == destPin.getId())
            return true;
    }

    return false;
}

Wire Generator::buildWire(NodeElement sourceElement, Pin sourcePin, NodeElement destElement, Pin destPin, WireType type)
{
    Wire wire(sourceElement.getElement().getIndex(), sourcePin.getId(), destElement.getElement().getIndex(), destPin.getId(), type, currentWireIndex);
    currentWireIndex ++;

    return wire;
}