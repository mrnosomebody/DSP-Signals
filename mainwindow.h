#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMdiArea>
#include <QMainWindow>
#include "qcustomplot.h"
#include "dialog.h"
#include <QVector>
#include <graph.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionChannel_Information_triggered();

    void SetRange();

    void ResetRange();

    void OpenGraphics(int);

    void on_actionAbout_triggered();

    void slotOscillogram(QAction*);

    void slotCustomMenuRequested(QPoint);

    void DefineFields(int);

    void on_actionModeling_triggered();

private:
    int modelationIndex = -1;
    QVector<QLineEdit*> lines;
    QVector<double> datalines;
    QLineEdit *ledit_1;
    QLineEdit *ledit_2;
    QDialog* dlg;
    bool modelingFlag;
    QDialog* modelingDialog;
    QFormLayout *modelingLayout;
    int createdSamplesNumber;
    double createdSamplingRate;
    int graphIndex;
    QVector<double> range;
    Ui::MainWindow *ui;
    QMdiArea* mdi;
    QMdiSubWindow* sub;
    QMdiSubWindow* modelationSub;
    QMdiSubWindow* sub_;
    Dialog* secwin;
    QVector <QString> str;
    QVector <double> xStatic;
    QCustomPlot* customPlot;
    QTableWidget *table;
    QWidget* o_widget;
    QWidget* modelingWidget;
    QVBoxLayout* modulationLayout;
    QVBoxLayout *o_layout_;
    QVector<Graph*> plots;
};
#endif // MAINWINDOW_H
