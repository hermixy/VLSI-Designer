#include <QtTest>
#include <QCoreApplication>
#include "util/renderer/gridrenderer.h"

class GridRendererTest : public QObject
{
    Q_OBJECT

    QString imagesDir;

public:
    GridRendererTest();
    ~GridRendererTest() {}

private slots:
    void renderTest();

    void renderEmptyTest();
    void renderPinTest();
    void renderElementTest();
    void renderWireTest();

private:
    QImage readImageFromFile(const QString filePath);
};

GridRendererTest::GridRendererTest()
{
    QDir dir (QApplication::applicationDirPath());
    imagesDir = dir.path() + "/images";
}

void GridRendererTest::renderTest()
{
    GridRenderer renderer;

    Grid* g = new Grid(10);

    QList<Cell> row1 = {Cell(CellType::Empty), Cell(CellType::Element, 0),
                Cell(CellType::Pin, 0 , "z"), Cell(CellType::DL)};
    QList<Cell> row2 = {Cell(CellType::Empty), Cell(CellType::DR),
                Cell(CellType::LR), Cell(CellType::UL)};
    QList<Cell> row3 = {Cell(CellType::Empty), Cell(CellType::UD),
                Cell(CellType::Pin, 1, "a1"), Cell(CellType::Element, 1)};
    QList<Cell> row4 = {Cell(CellType::Empty), Cell(CellType::UR),
                    Cell(CellType::Pin, 1, "a2"), Cell(CellType::Element, 1)};

    g->getCells().append(row1);
    g->getCells().append(row2);
    g->getCells().append(row3);
    g->getCells().append(row4);

    QImage img = renderer.render(g);    

    // uncomment to get new sample images if the resource images is changed
    // img.save("grid.png");
    QImage sample = readImageFromFile(imagesDir + "/grid.png");
    QVERIFY(img == sample);

    g->getCells()[3].removeAt(3);
    QVERIFY_EXCEPTION_THROWN(renderer.render(g), IllegalArgumentException);

    g->getCells().clear();
    QVERIFY_EXCEPTION_THROWN(renderer.render(g), IllegalArgumentException);

    delete g;
}

void GridRendererTest::renderEmptyTest()
{
    GridRenderer renderer;

    Grid* g = new Grid(10);

    QList<Cell> list = {Cell(CellType::Empty), Cell(CellType::Empty), Cell(CellType::Empty)};
    g->getCells().append(list);

    QImage img = renderer.render(g);

    // uncomment to get new sample images if the resource images is changed
    // img.save("grid_empty.png");
    QImage sample = readImageFromFile(imagesDir + "/grid_empty.png");
    QVERIFY(img == sample);

    delete g;
}

void GridRendererTest::renderPinTest()
{
    GridRenderer renderer;

    Grid* g = new Grid(10);

    QList<QList<Cell>> cells =
    {
        { Cell(CellType::Empty), Cell(CellType::Pin, 1, "a"), Cell(CellType::Empty),
          Cell(CellType::Empty), Cell(CellType::Empty) },
        { Cell(CellType::Pin, 1, "b"), Cell(CellType::Element, 1), Cell(CellType::Element, 1),
          Cell(CellType::Element, 1), Cell(CellType::Empty) },
        { Cell(CellType::Empty), Cell(CellType::Element, 1), Cell(CellType::Element, 1),
          Cell(CellType::Element, 1), Cell(CellType::Pin, 1, "c") },
        { Cell(CellType::Empty), Cell(CellType::Element, 1), Cell(CellType::Element, 1),
          Cell(CellType::Element, 1), Cell(CellType::Empty) },
        { Cell(CellType::Empty), Cell(CellType::Empty), Cell(CellType::Pin, 1, "d"),
          Cell(CellType::Empty), Cell(CellType::Empty) }
    };

    for(QList<Cell> list: cells)
        g->getCells().append(list);

    QImage img = renderer.render(g);

    // uncomment to get new sample images if the resource images is changed
    // img.save("grid_pins.png");
    QImage sample = readImageFromFile(imagesDir + "/grid_pins.png");
    QVERIFY(img == sample);

    delete g;
}

void GridRendererTest::renderElementTest()
{
    GridRenderer renderer;

    Grid* g = new Grid(10);

    QList<QList<Cell>> cells =
    {
        { Cell(CellType::Element, 1), Cell(CellType::Element, 1), Cell(CellType::Element, 1),
          Cell(CellType::Empty), Cell(CellType::Element, 2) },
        { Cell(CellType::Empty), Cell(CellType::Empty), Cell(CellType::Empty),
          Cell(CellType::Empty), Cell(CellType::Element, 2) },
        { Cell(CellType::Element, 3), Cell(CellType::Element, 3), Cell(CellType::Element, 3),
          Cell(CellType::Empty), Cell(CellType::Element, 2)},
        { Cell(CellType::Element, 3), Cell(CellType::Element, 3), Cell(CellType::Element, 3),
          Cell(CellType::Empty), Cell(CellType::Empty) },
        { Cell(CellType::Element, 3), Cell(CellType::Element, 3), Cell(CellType::Element, 3),
          Cell(CellType::Empty), Cell(CellType::Element, 4) }
    };

    for(QList<Cell> list: cells)
        g->getCells().append(list);

    QImage img = renderer.render(g);

    // uncomment to get new sample images if the resource images is changed
    //  img.save("grid_elements.png");

    QImage sample = readImageFromFile(imagesDir + "/grid_elements.png");
    QVERIFY(img == sample);

    delete g;
}

void GridRendererTest::renderWireTest()
{
    GridRenderer renderer;

    Grid* g = new Grid(10);

    QList<QList<Cell>> cells =
    {
        { Cell(CellType::Empty), Cell(CellType::Empty), Cell(CellType::Empty), Cell(CellType::LRU), Cell(CellType::Empty) },
        { Cell(CellType::LR), Cell(CellType::DL), Cell(CellType::UDL), Cell(CellType::Empty), Cell(CellType::UDR) },
        { Cell(CellType::Empty), Cell(CellType::UR), Cell(CellType::DL), Cell(CellType::LRD), Cell(CellType::Empty) },
        { Cell(CellType::Empty), Cell(CellType::DR), Cell(CellType::UL), Cell(CellType::UD), Cell(CellType::Empty) },
        { Cell(CellType::UR), Cell(CellType::UDLR), Cell(CellType::UDL), Cell(CellType::Empty), Cell(CellType::Empty) }
    };

    for(QList<Cell> list: cells)
        g->getCells().append(list);

    QImage img = renderer.render(g);

    // uncomment to get new sample images if the resource images is changed
    // img.save("grid_wires.png");

     QImage sample = readImageFromFile(imagesDir + "/grid_wires.png");
     QVERIFY(img == sample);

    delete g;
}

QImage GridRendererTest::readImageFromFile(const QString filePath)
{
    QFile f(filePath);
    QFileInfo info(filePath);

    if(!f.open(QIODevice::ReadOnly))
        throw IOException(QString("The resource image file cannot be found or opened: %1").arg(info.fileName()));

    QImage image = QImage::fromData(f.readAll(), info.suffix().toLocal8Bit());
    f.close();

    if(image.isNull())
        throw Exception(QString("The resource image is not supported or corrupted.").arg(info.fileName()));

    return image;
}

QTEST_MAIN(GridRendererTest)

#include "tst_gridrenderertest.moc"