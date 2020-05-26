#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QMainWindow>

#include "scanpage.h"
#include "testpage.h"
#include "spjcomm.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum SimMode{
        SimScaner = 0,
        SimSPJ = 1
    };
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void _initUi();
    void _setupCentralWidget(QWidget *parent);


    ScanPage* scanPage;
    TestPage* testPage;

    QAction *actAbout;

    SPJComm* _com;
};
#endif // MAINWINDOW_H
