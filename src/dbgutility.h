#ifndef DBGUTILITY_H
#define DBGUTILITY_H
#include <QDebug>

#define TRACE()    qDebug() << "[" << __FUNCTION__ << "(...)]"

#define IF_RETURN(condition)    do{     \
        if(condition){  return; }       \
    }while(0)

#define IF_RETURN_VAL(condition, val)   do{     \
    if(condition){return val;}                  \
    }while(0)

#define IF_THEN(condition, ...)     {       \
        if (condition){__VA_ARGS__ ;}        \
    }

#define IF_NOTIFY_RETURN(condition, text)   do{ \
        if(condition){                          \
            TRACE() << text;                    \
            QMessageBox::warning(this, tr("warning"),text); \
            return;                             \
        }                                       \
    }while(0)

#define RETURN_VAL_IF(rule, val) { if(rule) return val;}


#endif // DBGUTILITY_H
