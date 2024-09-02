#include "dialog1.h"
#include "ui_dialog1.h"

Dialog1::Dialog1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog1)
{

    ui->setupUi(this);
    setFixedSize(800,480);
}

Dialog1::~Dialog1()
{
    delete ui;
}

void Dialog1::HandleSignal(QString str){
        QFile file;
        file.setFileName("1.txt");
        //只读打开文件
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "open read file error: " << file.errorString();
            return;
        }
        QTextStream in(&file);
        //一行一行读取文件内容
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if(line == "\n" || line == NULL)
            {
                continue;
            }
            else
            {
                if(QString::compare(line,str)==0)
                {
                    ui->label_2->setText(str);
                    ui->label_3->setText("欢迎主人回家！");
                    show();
                    return;
                }



            }
        }
      /**/
        ui->label_2->setText(str);
        ui->label_3->setText("陌生人！不许进！");
           show();

    }

void Dialog1::on_pushButton_clicked()
{
      close();
}
