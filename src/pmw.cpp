#include "pmw.h"
#include "ui_pmw.h"

PMW::PMW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PMW)
{
    ui->setupUi(this);
}

PMW::~PMW()
{
    delete ui;
}
