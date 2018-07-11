#include "dialog.h"
#include "ui_dialog.h"
#include <iostream>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    std::cout << "dialog begin." << std::endl;
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    std::cout << "dialog desdroyed." << std::endl;
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    QString s = ui->textEdit->toPlainText();
    getText(s);
    this->~Dialog();
}

void Dialog::on_pushButton_2_clicked()
{
    getText(NULL);
    this->~Dialog();
}
