#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QWidget>
#include <QMdiSubWindow>
#include <Qt>
#include <QScrollBar>
#include <QMessageBox>
#include <cmath>

//int getRandomNumber(int min, int max)
//{
//    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
//    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
//}
int getRandomNumber(int a, int b){
   return (rand() % (b - a + 1) + a);
}


int counter = 0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mdi = new QMdiArea(this);
    mdi->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdi);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::OpenGraphics(int starting_point){
    int a = starting_point-6;          /*//////////////////////////////////////////////////////////////////////////////////*/
    customPlot = new QCustomPlot();
    customPlot->setFixedHeight(100);
    customPlot->xAxis->setTicks(false);
    customPlot->yAxis->setTicks(false);
    customPlot->xAxis->setTickLabels(false);
    customPlot->yAxis->setTickLabels(false);
    customPlot->xAxis->setLabel(str[5].split(';')[counter]);

    QWidget* widget = new QWidget();
    QVBoxLayout *layout_ = new QVBoxLayout();
    layout_->addWidget(customPlot);
    widget->setLayout(layout_);
    this->table->setContextMenuPolicy(Qt::CustomContextMenu);
    this->table->setRowHeight(a,120);
    this->table->setCellWidget(a,0,widget);

    connect(this->table,SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));

    QVector<double> vec;
    for (int i =starting_point; i<str.size();i+=str[0].toInt()){
        vec.append(str[i].toDouble());
    }

    customPlot->addGraph()->setData(xStatic,vec);
    customPlot->rescaleAxes();
    counter++;

}

void MainWindow::on_actionOpen_triggered()
{
    QVector<QString>().swap(str);
    counter = 0;
    QFile file = QFileDialog::getOpenFileName(this,tr("Open"),"C:/Пользователи/bezso/Рабочий стол",tr("Text files (*.txt)"));

    if (file.exists()){
        file.open(QIODevice::ReadOnly| QIODevice::Text);
        QTextStream in(&file);
        for (int i=0;!in.atEnd();++i) {
            QStringList line = in.readLine().split(' ');
            line.removeAll("");
            if (*line.begin()!='#'){
                str.append(line);
            }
        }
        this->table = new QTableWidget(str[5].split(';').length(),1,this);
        this->table->setColumnWidth(0,280);
        sub = mdi->addSubWindow(table);
        sub->setGeometry(QRect(1045,5,295,790));
        sub->setWindowTitle("Channels");
        sub->setContentsMargins(1,1,1,5);
        xStatic= QVector<double>(str[1].toInt());
        for (int i =0; i<str[1].toInt();++i){
            xStatic[i] = i*(1/str[2].toDouble());
        }

        for (int i = 0; i<str[0].toInt();++i){
            MainWindow::OpenGraphics(i+6);
        }
        file.close();
        sub->show();
    }
}

void MainWindow::on_actionChannel_Information_triggered()
{
    if (str.length()>1){
        double hz = 1/str[2].toDouble();
        QString s_datetime = str[3] +"  "+ str[4];
        double duration = str[1].toInt()*hz; //v sekyndax
        QList sTime = str[4].split(":");

        int hours = duration / 3600;
        int min = duration / 60 - hours * 60;
        int sec = duration - hours * 3600 - min * 60;

        std::string dhoursstr = std::to_string(hours);
        std::string dminstr = std::to_string(min);
        std::string dsecstr = std::to_string(sec);
        hours += sTime[0].toInt();
        min +=  sTime[1].toInt();
        sec +=  sTime[2].toDouble();

        std::string daystr = str[3].toStdString();
        std::string hoursstr = std::to_string(hours);
        std::string minstr = std::to_string(min);
        std::string secstr = std::to_string(sec);

        std::string e_date =daystr+"  "+hoursstr+":"+minstr+":"+secstr;
        std::string Duration = dhoursstr+":"+dminstr+":"+dsecstr;
        secwin = new Dialog(this);
        secwin->SetLabels(str[0],str[1],hz,s_datetime,e_date,Duration);
        secwin->show();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox info;
    info.setText("This app was created by Alex Alexeev, Max Nemiro and Sergey Bezsonov. It allows you to process signals, make graphs");
    info.exec();

}

void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    QMenu* menu = new QMenu(this);
    QAction *oscillogram = new QAction("Oscillogram", this);
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(slotOscillogram(QAction*)));
    menu->addAction(oscillogram);
    menu->popup(sub->mapToGlobal(pos));
}

void MainWindow::slotOscillogram(QAction* oscillogram){
    graphIndex = this->table->currentIndex().row();
    if (mdi->subWindowList().length()<2){
        plots.clear();
        QGridLayout* grid = new QGridLayout();
        QPushButton* range_btn = new QPushButton("Set range");
        QPushButton* reset_btn = new QPushButton("Reset range");
        QObject::connect(reset_btn, SIGNAL(clicked()), this, SLOT(ResetRange()));
        QObject::connect(range_btn, SIGNAL(clicked()), this, SLOT(SetRange()));
        o_widget = new QWidget();         //for oscillogram
        o_layout_ = new QVBoxLayout();
        o_widget->setLayout(o_layout_);
        grid->addWidget(range_btn,1,1);
        grid->addWidget(reset_btn,1,2);
        o_layout_->addLayout(grid);

        sub_ = mdi->addSubWindow(o_widget);
        sub_->setAttribute(Qt::WA_DeleteOnClose);
        sub_->setGeometry(QRect(5,5,675,420));     //
        sub_->setWindowTitle("Oscillogram");       //
        sub_->setContentsMargins(5,5,5,5);         //

        Graph* o_customPlot = new Graph();
        o_customPlot->setIndex(graphIndex);
        o_customPlot->setFixedHeight(200);
        o_customPlot->xAxis->setTicks(true);
        o_customPlot->yAxis->setTicks(true);
        o_customPlot->xAxis->setTickLabels(true);
        o_customPlot->yAxis->setTickLabels(true);
        o_customPlot->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
        o_customPlot->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
        o_customPlot->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси
        o_customPlot->axisRect()->setRangeZoom(Qt::Horizontal);   // Включаем удаление/приближение только по горизонтальной оси

        o_customPlot->xAxis->setLabel(str[5].split(';')[graphIndex]);
        o_layout_->addWidget(o_customPlot);

        QVector<double> vec;
        for (int i = graphIndex+6; i<str.size();i+=str[0].toInt()){
            vec.append(str[i].toDouble());
        }

        if (range.length()<1){
            o_customPlot->addGraph()->setData(xStatic,vec);
        }else{
            QVector<double> choosed_x;
            for (int i = range[0]; i < range[1]+1; ++i){
                choosed_x.append(xStatic[i]);
            }

            QVector<double> choosed_y;
            for (int i = range[0]; i < range[1]+1; ++i){
                choosed_y.append(vec[i]);
            }

            o_customPlot->addGraph()->setData(choosed_x,choosed_y);
        }
        o_customPlot->rescaleAxes();
        plots.emplaceBack(o_customPlot);
        sub_->show();

    }else{
        Graph* o_customPlot = new Graph();
        o_customPlot->setIndex(graphIndex);
        o_customPlot->setFixedHeight(200);
        o_customPlot->xAxis->setTicks(true);
        o_customPlot->yAxis->setTicks(true);
        o_customPlot->xAxis->setTickLabels(true);
        o_customPlot->yAxis->setTickLabels(true);
        o_customPlot->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
        o_customPlot->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
        o_customPlot->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси
        o_customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
        o_customPlot->xAxis->setLabel(str[5].split(';')[graphIndex]);
        o_layout_->addWidget(o_customPlot);

        QVector<double> vec;
        for (int i = graphIndex+6; i<str.size();i+=str[0].toInt()){
            vec.append(str[i].toDouble());
        }

        if (range.length()<1){
            o_customPlot->addGraph()->setData(xStatic,vec);
        }else{
            QVector<double> choosed_x;
            for (int i = range[0]; i < range[1]+1; ++i){
                choosed_x.append(xStatic[i]);
            }

            QVector<double> choosed_y;
            for (int i = range[0]; i < range[1]+1; ++i){
                choosed_y.append(vec[i]);
            }

            o_customPlot->addGraph()->setData(choosed_x,choosed_y);
        }
        o_customPlot->rescaleAxes();
        plots.emplaceBack(o_customPlot);
    }

}

void MainWindow::SetRange(){

    QDialog dlg(this);
    dlg.setWindowTitle(tr("Set range"));

    QLineEdit *ledit1 = new QLineEdit(&dlg);
    QLineEdit *ledit2 = new QLineEdit(&dlg);

    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(tr("Start point:"), ledit1);
    layout->addRow(tr("End point:"), ledit2);
    layout->addWidget(btn_box);

    dlg.setLayout(layout);

    if(dlg.exec() == QDialog::Accepted) {
        if (range.length()<1){
            range.append(ledit1->text().toInt());
            range.append(ledit2->text().toInt());
        }else{
            range[0] = ledit1->text().toInt();
            range[1] = ledit2->text().toInt();
        }

        QVector<double> choosed_x;
        for (int i = range[0]; i < range[1]+1; ++i){
            choosed_x.append(xStatic[i]);
        }

        for (int i = 0; i < plots.length(); ++i){
            int tmp = plots[i]->GetIndex();
            QVector<double> vec;
            for (int k = tmp+6; k<str.size();k+=str[0].toInt()){
                vec.append(str[k].toDouble());
            }
            QVector<double> choosed_y;
            for (int j = range[0]; j < range[1]+1; ++j){
                choosed_y.append(vec[j]);
            }
            plots[i]->~Graph();
            plots[i]= new Graph();
            plots[i]->setIndex(tmp);
            plots[i]->setFixedHeight(200);
            plots[i]->xAxis->setTicks(true);
            plots[i]->yAxis->setTicks(true);
            plots[i]->xAxis->setTickLabels(true);
            plots[i]->yAxis->setTickLabels(true);
            plots[i]->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
            plots[i]->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
            plots[i]->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси
            plots[i]->axisRect()->setRangeZoom(Qt::Horizontal);
            plots[i]->xAxis->setLabel(str[5].split(';')[tmp]);
            o_layout_->addWidget(plots[i]);
            plots[i]->addGraph()->setData(choosed_x,choosed_y);
            plots[i]->rescaleAxes();
        }
    }
}

void MainWindow::ResetRange(){
    if (range.length()>1){
        range.clear();
        for (int i = 0; i < plots.length(); ++i){
            int tmp = plots[i]->GetIndex();
            QVector<double> vec;
            for (int k = tmp+6; k<str.size();k+=str[0].toInt()){
                vec.append(str[k].toDouble());
            }

            plots[i]->~Graph();
            plots[i]= new Graph();
            plots[i]->setIndex(tmp);
            plots[i]->setFixedHeight(200);
            plots[i]->xAxis->setTicks(true);
            plots[i]->yAxis->setTicks(true);
            plots[i]->xAxis->setTickLabels(true);
            plots[i]->yAxis->setTickLabels(true);
            plots[i]->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
            plots[i]->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
            plots[i]->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси
            plots[i]->axisRect()->setRangeZoom(Qt::Horizontal);
            plots[i]->xAxis->setLabel(str[5].split(';')[tmp]);
            o_layout_->addWidget(plots[i]);
            plots[i]->addGraph()->setData(xStatic,vec);
            plots[i]->rescaleAxes();
        }
    }
}

void MainWindow::on_actionModeling_triggered()
{


    dlg = new QDialog(this);
    dlg->setWindowTitle(tr("Set data for modeling"));

    ledit_1 = new QLineEdit(dlg);
    ledit_2 = new QLineEdit(dlg);


    QDialogButtonBox *btn_box = new QDialogButtonBox(dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(btn_box, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, dlg, &QDialog::reject);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(tr("SamplesNumber:"), ledit_1);
    layout->addRow(tr("SamplingRate:"), ledit_2);
    layout->addWidget(btn_box);

    dlg->setLayout(layout);


    if(dlg->exec() == QDialog::Accepted ) {
        createdSamplesNumber = ledit_1->text().toInt();
        createdSamplingRate = ledit_2->text().toDouble();

        modelingDialog = new QDialog(this);
        modelingDialog->setWindowTitle(tr("Modeling"));

        QStringList items;
        items << tr("Discretized sinusoid with given") << tr("Saw") << tr("Exponential Envelope")\
              << tr("Balanced Envelope Signal") << tr("Tonal Envelope")\
              << tr("Linear frequency") << tr("Discretized decreasing exponent")<< tr("Meandr")\
              << tr("Delayed single pulse") << tr("Delayed single blink")\
              << tr("White Noise") << tr("Normal white noise") << tr("Random autoregression signal");

        QComboBox* combobox = new QComboBox();
        combobox->addItems(items);
        modelingLayout = new QFormLayout();
        modelingLayout->addRow(combobox);
        combobox->setCurrentIndex(0);
        connect(combobox, SIGNAL(currentIndexChanged(int)),this, SLOT(DefineFields(int)));
        QDialogButtonBox *btn_box1 = new QDialogButtonBox(modelingDialog);
        btn_box1->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        connect(btn_box1, &QDialogButtonBox::accepted, modelingDialog, &QDialog::accept);
        connect(btn_box1, &QDialogButtonBox::rejected, modelingDialog, &QDialog::reject);

        modelingLayout->addWidget(btn_box1);
        modelingDialog->setLayout(modelingLayout);
        if(modelingDialog->exec() == QDialog::Accepted ) {
            datalines.clear();
            for (int i = 0; i < lines.length(); ++i){
                datalines.append(lines[i]->text().toDouble());
                //qDebug()<<lines[i]->text().toDouble();
            }
            qDebug()<<datalines;
            QVector<double> modulationVecX;
            for (int i = 0; i < createdSamplesNumber+1; ++i){
                modulationVecX.append(i*(1/createdSamplingRate));
            }

            if (modelationIndex == 0){
                QVector<double> modulationVecY;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    modulationVecY.append(datalines[0]*sin(i*datalines[1]+datalines[2]));
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);


                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
                    graph->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
                    graph->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси
                    graph->axisRect()->setRangeZoom(Qt::Horizontal);   // Включаем удаление/приближение только по горизонтальной оси
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 1){
                QVector<double> modulationVecY;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    modulationVecY.append(i%static_cast<int>(datalines[0]));
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 2){ // [0] - amplituda , [1] - wirina ogibayweu , [2] - chastota nesyweu , [3] - nachalnaya chastota
                QVector<double> modulationVecY;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    //modulationVecY.append(datalines[0]*exp(-i/datalines[1])*cos(2*3.14*datalines[2]*i+datalines[3])); /////РИСУЕТ НЕ ТО ПАЛЬТО
                    modulationVecY.append(datalines[0]*pow(2.71,(-i/datalines[1]))*cos(2*M_PI*datalines[2]*i+datalines[3])); // ++ FIX экспоненциальная огибающая
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 3){
                QVector<double> modulationVecY;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    modulationVecY.append(datalines[0]*cos(2*3.14*datalines[1]*i)*cos(2*3.14*datalines[2]*i+datalines[3])); // ++ good
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 4){
                QVector<double> modulationVecY;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    modulationVecY.append(datalines[1]*(1+datalines[0]*cos(2*3.14*datalines[2]*i))*cos(2*3.14*datalines[3]*i+datalines[4])); // ++ good
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 5){
                QVector<double> modulationVecY;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    double t = 1;

                    modulationVecY.append(datalines[0] * cos(2 * M_PI * (datalines[1] + (datalines[2] - datalines[1])/i*i)/i + datalines[3])); //////Почти но хуйня
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 7){ // че нахуй?
                QVector<double> modulationVecY;
                int j = static_cast<int>(datalines[0]);
                for (int i = 1;i < createdSamplesNumber+1; ++i){
                    if (i % j < j/2){
                        modulationVecY.append(1);
                    }
                    else{
                        modulationVecY.append(-1);
                    }
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 8){
                QVector<double> modulationVecY;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    if (i == datalines[0]){
                        modulationVecY.append(1);
                    }
                    else{
                        modulationVecY.append(0);
                    }
                }

                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 9){
                QVector<double> modulationVecY;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    if(i < datalines[0]){
                        modulationVecY.append(0);
                    }
                    else{
                        modulationVecY.append(1);
                    }
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 10){
                QVector<double> modulationVecY;

                for (int i = 0; i < createdSamplesNumber+1; ++i){
                   modulationVecY.append(datalines[0]+(datalines[1] - datalines[0])*getRandomNumber(datalines[0],datalines[1]));
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 11){
                QVector<double> modulationVecY;
                double sum = 0;
                for (int i = 0; i < createdSamplesNumber+1; ++i){
                    double sum = 0;
                    for(int j = 0 ; j < 13; ++j){
                        sum +=getRandomNumber(datalines[0],datalines[1]) - 6;
                    }
                    modulationVecY.append(datalines[0] + datalines[1]*sum);
                }
                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }
            if (modelationIndex == 12){
                QVector<double> modulationVecY;
                double sum = 0; // eto nado
                double sum_1 = 0;
                double sum_2 = 0;
                double x_n = 0;
                for (int i = 0; i < createdSamplesNumber+1; ++i){ // etot cycle toje nyjen
                    double sum = 0;
                    for(int j = 0 ; j < 13; ++j){
                        sum +=getRandomNumber(0,datalines[0]*datalines[0]) - 6;
                    }
                    x_n = datalines[0] + datalines[1]*sum;



                    for (int q;q <= datalines[2];++q){
                        sum_1 =
                    }

                }

                if (mdi->subWindowList().length()<1){
                    modelingWidget = new QWidget();
                    modulationLayout = new QVBoxLayout();
                    modelingWidget->setLayout(modulationLayout);

                    modelationSub = mdi->addSubWindow(modelingWidget);
                    modelationSub->setAttribute(Qt::WA_DeleteOnClose);
                    modelationSub->setGeometry(QRect(5,5,675,420));
                    modelationSub->setWindowTitle("Modulation");
                    modelationSub->setContentsMargins(5,5,5,5);

                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);

                    modelationSub->show();
            }else{
                    Graph* graph = new Graph();
                    graph->setFixedHeight(200);
                    graph->xAxis->setTicks(true);
                    graph->yAxis->setTicks(true);
                    graph->xAxis->setTickLabels(true);
                    graph->yAxis->setTickLabels(true);
                    graph->addGraph()->setData(modulationVecX,modulationVecY);
                    graph->rescaleAxes();
                    modulationLayout->addWidget(graph);
                }
            }



        }
    }
}

void MainWindow::DefineFields(int a){
    QLayoutItem* item;
    if (modelingFlag == 1){
        while ((item = modelingLayout->takeAt(2)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }
    if (a==6){ // дискрет. убыв экспонента
        lines.clear();
        QLineEdit *ledit1 = new QLineEdit(modelingDialog);
        ledit1->setPlaceholderText("Insert 0<a<1");
        modelingLayout->addRow(ledit1);
        modelingFlag = 1;
        lines.append(ledit1);
        modelationIndex = 6;
    }
    if (a==0){ // синусоида
        lines.clear();
        QLineEdit *ledit1 = new QLineEdit(modelingDialog);
        QLineEdit *ledit2 = new QLineEdit(modelingDialog);
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        ledit1->setPlaceholderText("Amplitude");
        ledit2->setPlaceholderText("Сircular frequency [0,pi]");
        ledit3->setPlaceholderText("Start phase [0,2pi]");
        modelingLayout->addRow(ledit1);
        modelingLayout->addRow(ledit2);
        modelingLayout->addRow(ledit3);
        modelingFlag = 1;
        lines.append(ledit1);
        lines.append(ledit2);
        lines.append(ledit3);
        modelationIndex = 0;
    }
    if (a==1){ // пила
        lines.clear();
        QLineEdit *ledit1 = new QLineEdit(modelingDialog);
        ledit1->setPlaceholderText("Period");
        modelingLayout->addRow(ledit1);
        modelingFlag = 1;
        lines.append(ledit1);
        modelationIndex = 1;
    }
    if (a==2){ // экспоненциальная огибающая
        lines.clear();
        QLineEdit *ledit2 = new QLineEdit(modelingDialog);
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        QLineEdit *ledit4 = new QLineEdit(modelingDialog);
        QLineEdit *ledit5 = new QLineEdit(modelingDialog);
        ledit2->setPlaceholderText("Amplitude");
        ledit3->setPlaceholderText("Envelope width");
        ledit4->setPlaceholderText("Сarrier frequency");
        ledit5->setPlaceholderText("Start carrier phase");
        modelingLayout->addRow(ledit2);
        modelingLayout->addRow(ledit3);
        modelingLayout->addRow(ledit4);
        modelingLayout->addRow(ledit5);
        modelingFlag = 1;
        lines.append(ledit2);
        lines.append(ledit3);
        lines.append(ledit4);
        lines.append(ledit5);
        modelationIndex = 2;
    }
    if (a==3){ // балансная огибающая
        lines.clear();
        QLineEdit *ledit2 = new QLineEdit(modelingDialog);
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        QLineEdit *ledit4 = new QLineEdit(modelingDialog);
        QLineEdit *ledit5 = new QLineEdit(modelingDialog);
        ledit3->setPlaceholderText("Amplitude");
        ledit4->setPlaceholderText("Envelope frequency");
        ledit5->setPlaceholderText("Сarrier frequency");
        ledit2->setPlaceholderText("Start carrier frequency");
        modelingLayout->addRow(ledit3);
        modelingLayout->addRow(ledit4);
        modelingLayout->addRow(ledit5);
        modelingLayout->addRow(ledit2);
        modelingFlag = 1;
        lines.append(ledit3);
        lines.append(ledit4);
        lines.append(ledit5);
        lines.append(ledit2);
        modelationIndex = 3;
    }
    if (a==4){ // тональная огибающая
        lines.clear();
        QLineEdit *ledit2 = new QLineEdit(modelingDialog);
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        QLineEdit *ledit4 = new QLineEdit(modelingDialog);
        QLineEdit *ledit5 = new QLineEdit(modelingDialog);
        QLineEdit *ledit6 = new QLineEdit(modelingDialog);
        ledit2->setPlaceholderText("Modulation depth index");
        ledit3->setPlaceholderText("Amplitude");
        ledit4->setPlaceholderText("Envelope frequency");
        ledit5->setPlaceholderText("Сarrier frequency");
        ledit6->setPlaceholderText("Start carrier frequency");
        modelingLayout->addRow(ledit2);
        modelingLayout->addRow(ledit3);
        modelingLayout->addRow(ledit4);
        modelingLayout->addRow(ledit5);
        modelingLayout->addRow(ledit6);
        modelingFlag = 1;
        lines.append(ledit2);
        lines.append(ledit3);
        lines.append(ledit4);
        lines.append(ledit5);
        lines.append(ledit6);
        modelationIndex = 4;
    }
    if (a==5){ //линейная частотная модуляция ЛЧМ
        lines.clear();
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        QLineEdit *ledit4 = new QLineEdit(modelingDialog);
        QLineEdit *ledit5 = new QLineEdit(modelingDialog);
        QLineEdit *ledit6 = new QLineEdit(modelingDialog);
        ledit3->setPlaceholderText("Amplitude"); // +
        ledit4->setPlaceholderText("Frequency at the initial moment t = 0"); //
        ledit5->setPlaceholderText("End frequency");
        ledit6->setPlaceholderText("Initial phase");
        modelingLayout->addRow(ledit3);
        modelingLayout->addRow(ledit4);
        modelingLayout->addRow(ledit5);
        modelingLayout->addRow(ledit6);
        modelingFlag = 1;
        lines.append(ledit3);
        lines.append(ledit4);
        lines.append(ledit5);
        lines.append(ledit6);
        modelationIndex = 5;
    }
    if (a==7){ // meandr
        lines.clear();
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        ledit3->setPlaceholderText("Pheriod L"); // +
        modelingLayout->addRow(ledit3);
        modelingFlag = 1;
        lines.append(ledit3);
        modelationIndex = 7;
    }
    if (a==8){ // Задержанный единичный импульс
        lines.clear();
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        ledit3->setPlaceholderText("Delayed single impulse"); // +
        modelingLayout->addRow(ledit3);
        modelingFlag = 1;
        lines.append(ledit3);
        modelationIndex = 8;
    }
    if (a==9){ // Задержаный еденичный скачок
        lines.clear();
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        ledit3->setPlaceholderText("Delayed single blink"); // +
        modelingLayout->addRow(ledit3);
        modelingFlag = 1;
        lines.append(ledit3);
        modelationIndex = 9;
    }
    if (a==10){ // Белый шум
        lines.clear();
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        QLineEdit *ledit4 = new QLineEdit(modelingDialog);
        ledit3->setPlaceholderText("Bottom line"); // +
        ledit4->setPlaceholderText("Top line");
        modelingLayout->addRow(ledit3);
        modelingLayout->addRow(ledit4);
        modelingFlag = 1;
        lines.append(ledit3);
        lines.append(ledit4);
        modelationIndex = 10;
    }
    if (a==11){ // Белый шум ,произвольная нормальная величина
        lines.clear();
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        QLineEdit *ledit4 = new QLineEdit(modelingDialog);
        ledit3->setPlaceholderText("Middle value"); // +
        ledit4->setPlaceholderText("Dispersion");
        modelingLayout->addRow(ledit3);
        modelingLayout->addRow(ledit4);
        modelingFlag = 1;
        lines.append(ledit3);
        lines.append(ledit4);
        modelationIndex = 11;
    }
    if (a==12){ // Случайный сигнал авторегрессии
        lines.clear();
        QLineEdit *ledit3 = new QLineEdit(modelingDialog);
        QLineEdit *ledit4 = new QLineEdit(modelingDialog);
        QLineEdit *ledit5 = new QLineEdit(modelingDialog);
        QLineEdit *ledit6 = new QLineEdit(modelingDialog);
        QLineEdit *ledit7 = new QLineEdit(modelingDialog);
        ledit3->setPlaceholderText("Dispersion");
        ledit4->setPlaceholderText("Insert P");
        ledit5->setPlaceholderText("Insert Q");
        ledit6->setPlaceholderText("Insert A(whitespace)");
        ledit7->setPlaceholderText("Insert B(whitespace");

        modelingLayout->addRow(ledit3);
        modelingLayout->addRow(ledit4);
        modelingLayout->addRow(ledit5);
        modelingLayout->addRow(ledit6);
        modelingLayout->addRow(ledit7);

        modelingFlag = 1;
        lines.append(ledit3);
        lines.append(ledit4);
        lines.append(ledit5);
        lines.append(ledit6);
        lines.append(ledit7);
        modelationIndex = 12;
    }


}

