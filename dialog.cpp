#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

}

void Dialog::SetLabels(QString c_amount,QString s_amount,double s_rate,QString s_dt,std::string e_dt,std::string duration){
    this->ui->c_amount->setText(c_amount);
    this->ui->s_amount->setText(s_amount);
    this->ui->s_rate->setText(QString::fromUtf8(std::to_string(s_rate)));
    this->ui->s_dt->setText(s_dt);
    this->ui->e_dt->setText(QString::fromUtf8(e_dt));
    this->ui->duration->setText(QString::fromUtf8(duration));
}

Dialog::~Dialog()
{
    delete ui;
}

Ui::Dialog* Dialog::GetUi(){
    return ui;
}
