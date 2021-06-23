#include "graph.h"

void Graph::setIndex(int n){this->n=n;}

int Graph::GetIndex(){return this->n;}
//void Graph::setMdi(QMdiArea* mdi){this->mdi=mdi;}

//void Graph::setMainWindow(MainWindow* mainWindow){this->mainWindow=mainWindow;}

//void Graph::setSubWindow(QMdiSubWindow* sub_){this->sub_=sub_;}

//void Graph::setData(QVector<QString> str){this->str=str;}

//void Graph::setVectorX(QVector<double> xStatic){this->xStatic=xStatic;}

//void Graph::connect_(){
//    connect(this,SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
//}

//void Graph::slotCustomMenuRequested(QPoint pos)
//{
//    qDebug()<<"menu";
//    QMenu* menu = new QMenu(this);
//    QAction *oscillogram = new QAction("Oscillogram", mainWindow);
//    oscillogram->setProperty("num",n);
//    menu->addAction(oscillogram);
//    connect(menu, SIGNAL(triggered()), mainWindow, SLOT(slotOscillogram(oscillogram)));
//    menu->popup(this->mapToGlobal(pos));
//}
//void Graph::slotOscillogram(QAction* oscillogram){
//    int tmp = oscillogram->property("num").toInt();
//    //if (sub_==nullptr){
//        //qDebug()<<"ddadadadad";
//        QWidget* o_widget = new QWidget();         //for ocsillogram
//        QVBoxLayout* o_layout_ = new QVBoxLayout();
//        o_widget->setLayout(o_layout_);            //
//        sub_ = mdi->addSubWindow(o_widget);
//        //sub_->setAttribute(Qt::WA_DeleteOnClose);
//        sub_->setGeometry(QRect(5,5,675,420));     //
//        sub_->setWindowTitle("Oscillogram");       //
//        sub_->setContentsMargins(5,5,5,5);         //
//    //}else{
//        //qDebug()<<"nenenen";
//    //}

//    qDebug()<<tmp;

//    Graph* o_customPlot = new Graph();
//    o_customPlot->setFixedHeight(200);
//    o_customPlot->xAxis->setTicks(true);
//    o_customPlot->yAxis->setTicks(true);
//    o_customPlot->xAxis->setTickLabels(true);
//    o_customPlot->yAxis->setTickLabels(true);
//    o_customPlot->xAxis->setLabel(str[5].split(';')[tmp]);
//    o_layout_->addWidget(o_customPlot);

//    QVector<double> vec;
//    for (int i =6; i<str.size();i+=str[0].toInt()){
//        vec.append(str[i].toDouble());
//    }
//    o_customPlot->addGraph()->setData(xStatic,vec);
//    o_customPlot->rescaleAxes();
//    sub_->show();
//}
