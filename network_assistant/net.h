#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <qdebug.h>
#include <iostream>
#include <QString>
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <pthread.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QDateTime>
#include <time.h>

using namespace std;

void*tcpToUi(void*that1);//tcp服务器模式下，接受连接的客户端传来的信息
int udpRecv(char*ip,int port,char*(&recv_buf1),MainWindow*that,int&sockfd);  //udp收
int udpSend(char *myip, int myport, char*ip, int port, char*(send_buf));  //udp发
int tcpClient(char*ip,int port,char* (&recv_buf1),MainWindow*that,int&sockfd);//tcp客户端
int tcpServer(char*ip, int port, char* (&recv_buf1), MainWindow*that, int&tcpfd2);//tcp服务器
void *tcpDealClient(void*arg);
void *SocketConnected(void*cli_msg1);//判断客户端连接状态线程

#endif // 网络通信的函数

