#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QWidget>
#include <QMdiSubWindow>
#include <Qt>
#include <QScrollBar>
#include <QMessageBox>


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
            xStatic[i] = i*str[2].toDouble();
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
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Set data for modeling"));

    QLineEdit *ledit1 = new QLineEdit(&dlg);
    QLineEdit *ledit2 = new QLineEdit(&dlg);

    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(tr("SamplesNumber:"), ledit1);
    layout->addRow(tr("SamplingRate:"), ledit2);
    layout->addWidget(btn_box);

    dlg.setLayout(layout);

    if(dlg.exec() == QDialog::Accepted) {
        createdSamplesNumber = ledit1->text().toInt();
        createdSamplingRate = ledit2->text().toDouble();

        modelingDialog = new QDialog(this);
        modelingDialog->setWindowTitle(tr("Modeling"));

        QStringList items;
        items << tr("Discretized sinusoid with given") << tr("Saw") << tr("Exponential Envelope")\
              << tr("Balanced Envelope Signal") << tr("Tonal Envelope")\
              << tr("Linear frequency") << tr("Discretized decreasing exponent");

//        QString item = QInputDialog::getItem(this, "www.itmathrepetitor.ru",tr("Предмет:"),
//                     items, 0, false, &ok);


        QComboBox* combobox = new QComboBox();
        combobox->addItems(items);
        modelingLayout = new QFormLayout();
        modelingLayout->addRow(combobox);
        QObject::connect(modelingDialog, SIGNAL(currentTextChanged(const QString &)),this, SLOT(DefineFields(const QString &)));
        qDebug()<<combobox->currentData();
//        if (combobox->currentData() == "Discretized decreasing exponent"){
//            QLineEdit *ledit1 = new QLineEdit(&dlg1);
//            QLineEdit *ledit2 = new QLineEdit(&dlg1);
//            ledit1->setPlaceholderText("Insert 0<a<1");
//            ledit2->setPlaceholderText("Insert n>=0");
//            layout->addRow(tr("a"), ledit1);
//            layout->addRow(tr("n"), ledit2);
//        }

        QDialogButtonBox *btn_box1 = new QDialogButtonBox(modelingDialog);
        btn_box1->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        connect(btn_box1, &QDialogButtonBox::accepted, modelingDialog, &QDialog::accept);
        connect(btn_box1, &QDialogButtonBox::rejected, modelingDialog, &QDialog::reject);

        layout->addWidget(btn_box1);

        modelingDialog->setLayout(modelingLayout);
        modelingDialog->exec();
    }
}

void MainWindow::DefineFields(int a){
    QLineEdit *ledit1 = new QLineEdit(modelingDialog);
    QLineEdit *ledit2 = new QLineEdit(modelingDialog);
    ledit1->setPlaceholderText("Insert 0<a<1");
    ledit2->setPlaceholderText("Insert n>=0");
    modelingLayout->addRow(tr("a"), ledit1);
    modelingLayout->addRow(tr("n"), ledit2);
}

