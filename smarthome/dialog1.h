#ifndef DIALOG1_H
#define DIALOG1_H
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QLabel>
#include <iostream>
#include <fstream>
#include <ostream>
#include <QFileDialog>

namespace Ui {
class Dialog1;
}

class Dialog1 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog1(QWidget *parent = nullptr);
    ~Dialog1();

private slots:
    void on_pushButton_clicked();
    void HandleSignal(QString str);

private:
    Ui::Dialog1 *ui;

};

#endif // DIALOG1_H
