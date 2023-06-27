#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QFileDialog>
#include <QSpinBox>
#include <QMessageBox>
#include "QVector"
#include "QDataStream"
#include "QFile"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "math.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <stdlib.h>
#include <time.h>
#include <QProcess>
#include <iomanip>
#include <QCheckBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct Signal
    {
        int id;
        u_int16_t energy;
        int reads;
    };

public:
    MainWindow();

    void readSettings();
    bool areClover;

    QStringList fileAddress;
    QStringList shortAddress;

    QString outAddress;

    QWidget* window;


    QLabel* fileName;
    QPushButton* fileNameButton;

    QLabel* outName;
    QPushButton* outNameButton;

    QLabel* areCloverLabel;
    QCheckBox* areCloverCheckBox;

    QPushButton* runButton;

    QLabel* progressLabel;

    void closeEvent(QCloseEvent *event);
    void writeSettings();

    void readData();
    uint16_t cache[8192];
    int written;
    int fileNumber;

    QDataStream output;
    QFile outputFile;

public slots:
    void fileButtonClicked();
    void runButtonClicked();
    void outButtonClicked();
    void cloverClicked();
    void mixEvents(QVector <Signal>);
    int writeCache();
};

#endif // MAINWINDOW_H
