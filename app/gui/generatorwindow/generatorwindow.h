#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QTime>

#include "config/config.h"
#include "generator/generator.h"
#include "util/serializer/serializerstrategy.h"

namespace Ui
{
    class GeneratorWindow;
}

class GeneratorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GeneratorWindow(Config& config, QWidget *parent = 0);
    ~GeneratorWindow();

protected:
     void closeEvent(QCloseEvent *event);

private slots:
    void on_librariesButton_clicked();
    void on_generateButton_clicked();
    void on_closeButton_clicked();
    void on_stopButton_clicked();

    void onSendScheme(Scheme* s);
    void onSendError(QString error);
    void onSendLog(QString log);
    void onFinish();

signals:
    void sendStop();

private:
    void setValidators();

    GeneratorParameters buildParameters();

    void saveScheme(Scheme* s);

    Ui::GeneratorWindow *ui;

    Config& config;

    QStringList libraryFiles;
    QList<Library*> libraries;
    Generator* generator;

    QThread generatorThread;
};
