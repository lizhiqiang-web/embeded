#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::handle_yw(int  num)//处理传输的烟雾数值
{
    QString str1 = QString::number(num);//将整型转化为字符串
    ui->label_9->setText(str1);//发送显示的见面去
    if((num>=yw_w_v)&&flag_yw_v==0)//大于则闹鸣
    {
        open_cbeep;
        flag_yw_v=1;
    }
    if((num<yw_w_v)&&flag_yw_v==1)//不闹
    {
        close_cbeep;
        close_beep;
        flag_yw_v=0;
    }

}
void MainWindow::handle_gy(QString str1,QString str2,QString str3)//处理传输的gy39的数值
{
    ui->label_7->setText(str1);//发送显示的见面去
    ui->label_3->setText(str2);
    ui->label_5->setText(str3);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     ui->setupUi(this);
     MyThread *t1 = new MyThread();
     gy_thread * gy =new gy_thread();
     qmyw_thread * ywqm =new qmyw_thread();
     gy->Gy39Init();
    connect(t1,SIGNAL(IdSignal(QString)),&dia,SLOT(HandleSignal(QString)));
    connect(ywqm,SIGNAL(send_yw(int)),this,SLOT(handle_yw(int)));
    connect(gy,SIGNAL(send_gy(QString,QString,QString)),this,SLOT(handle_gy(QString,QString,QString)));
    t1->start();
    gy->start();
    ywqm->start();
    setFixedSize(800,480);
    ShowAlarm();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_1_clicked()
{
      DeleteID(ui->listView->selectionModel()->currentIndex());
}
void MainWindow::on_pushButton_clicked()
{
    add_id();
}






void MainWindow::ShowAlarm()
{
    ItemModel = new QStandardItemModel(this);
    QStringList strList;
    QFile file;
    file.setFileName("1.txt");
    //打开文件
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
            strList.append(line);
        }
    }
     int nCount = strList.size();
     for(int  i = 0;i < nCount;i++)
     {
         string = static_cast<QString>(strList.at(i));
         QStandardItem *item = new QStandardItem(string);
         ItemModel->appendRow(item);
     }
     ui->listView->setModel(ItemModel);
     ui->listView->setModel(ItemModel);
         //connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(DeleteID(QModelIndex)));
}
void MainWindow::add_id()
{

    //alar_time=ui->timeEdit->time();
   //创建文件对象
   QFile file("1.txt");
   //读写打开文件
   bool ok = file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
   if(!ok)
   {
       qDebug() << "file open error:" << file.errorString();
       return ;
   }
   //写文件
   QTextStream out(&file);
   out << ui->lineEdit->text()<< endl;
   //关闭文件
   file.close();
   ShowAlarm();
}
//删除
void MainWindow::DeleteID(QModelIndex index)
{
    strTemp = index.data().toString();
    dialog1 = new QDialog(this);
    QLabel *label = new QLabel(dialog1);
    label->setText("确认删除吗？");
    label->move(20,30);
    QPushButton *yesBtn = new QPushButton("确定",dialog1);
    QPushButton *cancelBtn = new QPushButton("取消",dialog1);
    yesBtn->move(50,60);
    yesBtn->setFixedSize(50,35);
    cancelBtn->move(110,60);
    cancelBtn->setFixedSize(50,35);
    dialog1->setFixedSize(200,140);
    dialog1->setWindowTitle("id");

    connect(yesBtn,SIGNAL(clicked()),this,SLOT(Sure_delete()));
    connect(cancelBtn,SIGNAL(clicked()),this,SLOT(Cancel_delete()));
    dialog1->exec();
}

void MainWindow::Sure_delete()
{
    QFile file("1.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString qline;
     std::string line;
     ofstream outfile("2.txt",ios::out | ios::trunc);
    while(!in.atEnd())
    {
        qline = in.readLine();

        //比较结果不相等才执行outfile
        if(qline.compare(strTemp))
        {
            line = qline.toStdString();
            outfile << line << endl;
        }
    }


    outfile.close();
    file.close();
    ofstream outfile1("1.txt",ios::out | ios::trunc);
    fstream file1("2.txt");
    while(!file1.eof())
    {
        getline(file1,line);
        outfile1 << line << endl;
    }
    outfile1.close();
    file1.close();
    system(" del alarm2.txt");
    dialog1->close();
    ShowAlarm();
}
void MainWindow::Cancel_delete()
{
    dialog1->close();
}



void MainWindow::on_pushButton_2_clicked()
{
    open_led8;
}

void MainWindow::on_pushButton_8_clicked()
{
    close_led8;
}

void MainWindow::on_pushButton_3_clicked()
{
    open_led9;
}

void MainWindow::on_pushButton_9_clicked()
{
    close_led9;
}

void MainWindow::on_pushButton_4_clicked()
{
    open_led10;
}

void MainWindow::on_pushButton_10_clicked()
{
    close_led10;
}


void MainWindow::on_pushButton_6_clicked()
{

    if(flag_jiankong==0)
    {
        open_jiankong;
        ui->pushButton_6->setText("关闭监控");
    }
    if(flag_jiankong==1)
    {
        flag_jiankong=0;
        close_jiankong;
        ui->pushButton_6->setText("打开监控");
        return ;
    }
    flag_jiankong++;
}

void MainWindow::on_pushButton_5_clicked()
{
    if(flag_yuyinid==0)
    {
        open_yuyinid;
        ui->pushButton_5->setText("关闭语音");
    }
    if(flag_yuyinid==1)
    {
        flag_yuyinid=0;
        close_yuyinid;
        ui->pushButton_5->setText("打开语音");
        return ;
    }
    flag_yuyinid++;
}
