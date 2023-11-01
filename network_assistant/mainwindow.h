#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <QPixmap>
#include <QPalette>

using namespace std;

namespace Ui {
class MainWindow;
}

void*recv(void *that1);//监听线程
struct ClientMsg;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    friend void*recv(void *that1);  //接受线程
    friend int udpRecv(char*ip,int port,char*(&recv_buf1),MainWindow*that,int&sockfd);
    friend int tcpClient(char*ip,int port,char*(&recv_buf1),MainWindow*that,int&sockfd);
    friend int tcpServer(char*ip,int port,char*(&recv_buf1),MainWindow*that,int&tcpfd2);
    friend void *tcpDealClient(void*arg);
    friend struct ClientMsg;
    friend void *SocketConnected(void*cli_msg1);
    friend void*tcpToUi(void*that1);
    std::map<string,int> fdMap;   //存入连接tcp服务器的客户端的套接字和ip

    ~MainWindow();

private:
    Ui::MainWindow *ui;


private slots:
    void on_radioButton_toggled(bool checked);

    void on_pushButton_2_clicked();
    void on_comboBox_activated(int index);

    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_clicked();
};
struct ClientMsg    //已连接服务器的客户端的信息
{
    char ip_port[30];
    MainWindow*that;
    int cli_fd;
};

#endif // MAINWINDOW_H
