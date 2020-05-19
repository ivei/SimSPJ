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
{
    _initUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::_initUi()
{
    this->resize(400,300);
    this->actAbout = new QAction("关于");
    // 初始化菜单
    QMenu *menuHelp = new QMenu(tr("帮助"));
    menuHelp->addAction(this->actAbout);
    this->menuBar()->addMenu(menuHelp);

    // 初始化状态栏
    QStatusBar *statusBar = new QStatusBar;
    this->setStatusBar(statusBar);

    QWidget *central = new QWidget(this);
    this->setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    QToolBox *toolBox = new QToolBox(this);
    mainLayout->addWidget(toolBox);
    QToolButton *btn1 = new QToolButton;
    btn1->setDefaultAction(actAbout);
    toolBox->addItem(btn1);
    mainLayout->addWidget(new QTableWidget);
}

