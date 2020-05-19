#ifndef PMW_H
#define PMW_H

#include <QMainWindow>

namespace Ui {
class PMW;
}

class PMW : public QMainWindow
{
    Q_OBJECT

public:
    explicit PMW(QWidget *parent = nullptr);
    ~PMW();

private:
    Ui::PMW *ui;
};

#endif // PMW_H
