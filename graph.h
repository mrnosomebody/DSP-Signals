#ifndef GRAPH_H
#define GRAPH_H
#include "qcustomplot.h"


class Graph:public QCustomPlot
{

public:


    void setIndex(int);

    int GetIndex();

    //void setMainWindow(MainWindow*);

    //void setSubWindow(QMdiSubWindow*);

    //void setData(QVector<QString>);

    //void setVectorX(QVector <double>);

    //void setMdi(QMdiArea*);

    //void connect_();

private slots:

    //void slotCustomMenuRequested(QPoint);

    //void slotOscillogram(QAction * );
private:
    int n;
//    QMdiArea* mdi;
//    MainWindow* mainWindow;
//    QMdiSubWindow* sub_;
//    QVector <QString> str;
//    QVector <double> xStatic;
};

#endif // GRAPH_H
