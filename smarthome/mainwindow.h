#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
extern "C"{
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <netdb.h>
}
#include <string.h>
#include <errno.h>
#include <QString>
#include <QMessageBox>
#include <math.h>
#include <QDebug>
#include <QStandardItemModel>
#include <QFile>
#include <QLabel>
#include <iostream>
#include <fstream>
#include <ostream>
#include <QFileDialog>
#include <QTextStream>
#include <cstdlib>
#include <QDialog>
#include <string>
#include <dialog1.h>
#include <mythread.h>
#include<gy_thread.h>
#include<qmyw_thread.h>


#define  close_all_led system("/my_project/bin/led_all_close")
#define  open_led8    system("/my_project/bin/open_led8")
#define  close_led8   system("/my_project/bin/close_led8")
#define  open_led9    system("/my_project/bin/open_led9")
#define  close_led9    system("/my_project/bin/close_led9")
#define  open_led10    system("/my_project/bin/open_led10")
#define  close_led10    system("/my_project/bin/close_led10")
#define  open_cbeep    system("/my_project/bin/open_cbeep")
#define  close_cbeep    system("killall -9 open_cbeep")
#define  close_beep    system("/my_project/bin/close_beep")
#define  open_jiankong  system("/my_project/bin/open_jiankong")
#define  close_jiankong  system("killall -9 /my_project/bin/open_jiankong")
#define  open_yuyinid   system("/my_project/bin/open_yuyinid")
#define  close_yuyinid  system("killall -9 /my_project/bin/open_yuyinid")


using namespace std;



QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void add_id();




private slots:
    void handle_yw(int num);
    void handle_gy(QString str1,QString str2,QString str3);
    void Sure_delete();
    void Cancel_delete();
    void on_pushButton_1_clicked();
    void on_pushButton_clicked();
    void DeleteID(QModelIndex index);
    void on_pushButton_2_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_10_clicked();


    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *ItemModel;
    QDialog *dialog1;
    QString strTemp;
    QString str_id;
    QString string;
    Dialog1 dia;
    int yw_w_v=50;
    char flag_yw_v=0;
    char flag_jiankong=0;
    char flag_yuyinid=0;
    void ShowAlarm();
};
#endif // MAINWINDOW_H
