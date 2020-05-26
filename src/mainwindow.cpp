#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QToolBox>
#include <QListWidget>
#include <QTableWidget>

#include "mainwindow.h"
#include "dbgutility.h"

#pragma execution_character_set("utf-8")


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _com(new SPJComm(this))
{
    _initUi();
}

MainWindow::~MainWindow()
{
    delete _com;
}

void MainWindow::_initUi()
{
    this->resize(800,600);
    this->actAbout = new QAction("关于");
    // 初始化菜单
    QMenu *menuHelp = new QMenu(tr("帮助"));
    menuHelp->addAction(this->actAbout);
    this->menuBar()->addMenu(menuHelp);

    // 初始化状态栏
    QStatusBar *statusBar = new QStatusBar;
    this->setStatusBar(statusBar);

    //ScanPage *central = new ScanPage(this->_com, this);
    TestPage *central = new TestPage();
    this->setCentralWidget(central);



}



