#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    void SetLabels(QString,QString,double,QString,std::string,std::string);
    ~Dialog();
    Ui::Dialog* GetUi();
private:
    Ui::Dialog *ui;

};

#endif // DIALOG_H
