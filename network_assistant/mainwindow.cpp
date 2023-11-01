#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "net.h"

char*ip;    //本地ip
int port;   //本地端口
char *recv_buf=new char[128];//接受缓冲区
int menu=0;   //模式
int udpfd=-1;  //udp套接字
int tcpfd=-1;  //tcp用的套接字
pthread_t tid=0;    //启动TCP/IP监听的线程号
char *pic[7]={":/防火女.png",":/雨窗.jpg",":/天空.jpg",":/缤纷叶.jpg"\
            ":/涂白.jpg",":/精灵.png",":/灰木板.jpg"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->frame->setStyleSheet("QFrame{background: #e0fbff}");

//    this->setAutoFillBackground(true);//允许绘制
//    //创建图片控件(指定资源和大小)
//    QPixmap pix = QPixmap(pic[6]).scaled(this->size());
//    //创建调色板类
//    QPalette palette;
//    palette.setBrush(QPalette::Background,QBrush(pix));//给调色板 加载图片
//    this->setPalette(palette);//给窗口设置调色板
}

void *tcpToUi(void *that1)
{
    MainWindow*that=(MainWindow*)that1;

    mkfifo("myfifo",O_RDWR|O_CREAT);
    int fd=open("myfifo",O_RDONLY);
    if(fd<0)
    {
        perror("tcpToUi: open:");
        return NULL;
    }
    while(1)
    {
        char buf[512]="";
        memset(buf,0,512);
        int len=read(fd,buf,sizeof(buf));
        if(len>0)
        {
            QString qsBuf(buf);
            that->ui->textBrowser->append(qsBuf);
        }

    }

    close(fd);
    return NULL;
}

void *recv(void *that1) //TCP/IP 套接字创建/监听线程
{
    MainWindow*that=(MainWindow*)that1;
    QByteArray ba=that->ui->lineEdit->text().toLatin1();//获取左上角输入框ip
    ip=ba.data();
    QByteArray ba2=that->ui->lineEdit_2->text().toLatin1();//获取左上角输入框端口
    char*port_str=ba2.data();
    port=atoi(port_str);
    pthread_t tid1=0;
    //根据模式选择输入框进入不同的处理模式
    switch (menu) {
    case 0: //udp recv

        udpRecv(ip,port,recv_buf,that,udpfd);
        break;

    case 1://tcp client
        tcpClient(ip,port,recv_buf,that,tcpfd);
        break;

    case 2: //tcp server
        pthread_create(&tid1,NULL,tcpToUi,that1);
        pthread_detach(tid1);
        tcpServer(ip,port,recv_buf,that,tcpfd);
        break;
    default:
        break;
    }

    qDebug()<<"recv:线程即将退出";
    pthread_exit(NULL);
    return NULL;
}

MainWindow::~MainWindow()
{
    delete []recv_buf;
    ::close(udpfd);
    ::close(tcpfd);
    delete ui;
}

void MainWindow::on_radioButton_toggled(bool checked)//开关网络设置连接
{

    if(checked) //按钮按下，网络调试打开，开启连接服务（UＤＰ，ＴＣＰ），并将窗口变灰，不可输入
    {
        //创建线程进行监听收信
        int ret=pthread_create(&tid,NULL,recv,this);
        if(ret!=0)
        {
            perror("pthread_create");
            return;
        }
        cout<<"线程已创建"<<endl;
        pthread_detach(tid);
        //改变窗口状态
        ui->groupBox->setEnabled(false);
        ui->frame->setStyleSheet("background-color:#dbffdd");

    }

    else    //关闭连接，并将窗口变回正常
    {
        ui->groupBox->setEnabled(true);
        ui->frame->setStyleSheet("background-color:#e0fbff}");

        int ret=pthread_cancel(tid);//取消监听线程
        ::close(udpfd);
        ::close(tcpfd);
        qDebug()<<"close udpfd:"<<udpfd<<" tcpfd:"<<tcpfd;
        if(ret!=0)
        {
            perror("pthread_cancel");
        }
        else
        {
             qDebug()<<"线程已取消"<<endl;
        }
    }
    return;
}

void MainWindow::on_pushButton_2_clicked()//发送按钮点击
{
    QByteArray ba;
    QByteArray ba2;
    QByteArray ba4;
    QByteArray ba5;
    QByteArray ba3=ui->textEdit->toPlainText().toUtf8();//获取发送内容
    char*port_str;
    char*myport_str;
    char*send_buf=ba3.data();
    char *myip;
    int myport;
    string strIp;
    map<string,int>::const_iterator it=fdMap.begin();
    switch(menu){

        case 0: //udp发送
            ba=ui->lineEdit_3->text().toLatin1();//获取右下角输入框目的ip
            ip=ba.data();
            ba2=ui->lineEdit_5->text().toLatin1();//获取右下角输入框目的端口
            port_str=ba2.data();
            port=atoi(port_str);

            ba5=ui->lineEdit->text().toLatin1();//获取左上角输入框本机ip
            myip=ba5.data();
            ba4=ui->lineEdit_2->text().toLatin1();//获取左上角输入框本机端口
            myport_str=ba4.data();
            myport=atoi(myport_str);
            qDebug()<<"发送按钮：目的"<<ip<<":"<<port;
            udpSend(myip,myport,ip,port,send_buf);//udp发送
            break;

        case 1: //tcp client发送
            qDebug()<<"tcpfd:"<<tcpfd<<" "<<"tcp client send:"<<send_buf;
            send(tcpfd,send_buf,strlen(send_buf),0);
            break;

        case 2: //tcp server发送
                if(ui->comboBox_2->currentIndex()==0)//选择发给所有客户端
                {
                   for(;it!=fdMap.end();it++)
                    {
                        send(it->second,send_buf,strlen(send_buf),0);
                    }
                   break;
                }
                strIp=ui->comboBox_2->currentText().toStdString();
                qDebug()<<"准备发送clifd："<<fdMap[strIp];
                send(fdMap[strIp],send_buf,strlen(send_buf),0);
            break;

        default:
            break;
    }

}

void MainWindow::on_comboBox_activated(int index)
{//模式切换
     menu=index;  //选项索引值0:UDP　１:TCP客户端 2:TCP服务器
     if(menu==1)
     {
         ui->stackedWidget->setCurrentIndex(2);
         ui->label_2->setText("远程主机地址");
         ui->label_3->setText("远程主机端口");
     }
     else if(menu==2)
     {
         ui->stackedWidget->setCurrentIndex(1);
     }
     else
     {
         ui->stackedWidget->setCurrentIndex(0);
         ui->label_2->setText("本地主机地址");
         ui->label_3->setText("本地主机端口");
     }
}

void MainWindow::on_pushButton_4_clicked()
{//清除日志
    ui->textBrowser->clear();
}

void MainWindow::on_pushButton_5_clicked()
{//清除输入框
    ui->textEdit->clear();
}

void MainWindow::on_pushButton_clicked()
{//清楚ip和端口
    ui->lineEdit_3->clear();
    ui->lineEdit_5->clear();
}
