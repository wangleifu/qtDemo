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
    QString s1 = ui->textEdit->toPlainText();
    QString s2 = ui->textEdit_2->toPlainText();
    std::cout << "send signals...\n" << "s1: " << s1.toStdString() << " s2: " << s2.toStdString() << std::endl;
    getText(s1, s2);
    this->~Dialog();
}
