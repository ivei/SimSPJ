#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QMainWindow>
#include <QTabWidget>

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


    QAction *actAbout;
};
#endif // MAINWINDOW_H
