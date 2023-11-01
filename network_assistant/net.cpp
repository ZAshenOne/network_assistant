#include "net.h"

int udpRecv(char *ip, int port, char* (&recv_buf1), MainWindow*that, int &sockfd)
{
    cout<<"接受udp进入"<<endl;
    cout<<ip<<"  "<<port<<endl;
    //创建套接字
    sockfd= socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd<0)
    {
        perror("sockfd:");
        return -1;
    }
    int optval=1;
     setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));
    //定义结构体
    struct sockaddr_in my_addr;
    bzero(&my_addr,sizeof(my_addr));
    my_addr.sin_family=AF_INET;       //IPV4
    my_addr.sin_port=htons(port);    //哪个端口收
    inet_pton(AF_INET,ip,&my_addr.sin_addr.s_addr); //IPV4地址
    int ret=bind(sockfd,(struct sockaddr*)&my_addr,sizeof(my_addr));
    if(ret<0)
    {
        perror("bind");
        close(sockfd);
        return -1;
    }
    //接受消息
    while(1)
    {
        cout<<"等待接受"<<endl;
        memset(recv_buf1,0,128);
        //获取时间
        QDateTime datetime=QDateTime::currentDateTime();
        QString timeStr=datetime.toString("yyyy-MM-dd hh:mm:ss");
        QByteArray ba=timeStr.toLatin1();
        char*time_buf=ba.data();
        sprintf(recv_buf1,"[%s]:\n",time_buf);

        recvfrom(sockfd,recv_buf1+strlen(recv_buf1),128-strlen(recv_buf1),0,NULL,NULL);
        qDebug()<<"准备显示到控件";
        qDebug()<<recv_buf1;
        QString recvbuf(recv_buf1);
        qDebug()<<"已转为QString："<<recvbuf;
        that->ui->textBrowser->append(recvbuf);
    }

    return 0;
}
int udpSend(char*myip,int myport,char *ip, int port, char *send_buf)
{
    //创建套接字
    int sockfd= socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd<0)
    {
        perror("sockfd:");
        return 0;
    }
    //定义目标结构体
    struct sockaddr_in dst_addr;
    bzero(&dst_addr,sizeof(dst_addr));
    dst_addr.sin_family=AF_INET;       //IPV4
    dst_addr.sin_port=htons(port);    //发往哪个端口
    inet_pton(AF_INET,ip,&dst_addr.sin_addr.s_addr); //IPV4地址
    //定义本机结构体
    struct sockaddr_in my_addr;
    bzero(&my_addr,sizeof(my_addr));
    my_addr.sin_family=AF_INET;       //IPV4
    my_addr.sin_port=htons(myport);    //固定端口发
    inet_pton(AF_INET,myip,&my_addr.sin_addr.s_addr); //IPV4地址
    qDebug()<<"udpSend:本机ip　port"<<myip<<myport;
    bind(sockfd,(struct sockaddr*)&my_addr,sizeof(my_addr));
    //发送数据
    qDebug()<<"udpSend:目的ip　port"<<ip<<port;
    sendto(sockfd,send_buf,strlen(send_buf),0,(struct sockaddr *)&dst_addr,sizeof(dst_addr));

    //关闭套接字
    close(sockfd);
    return 0;
}

int tcpClient(char *ip, int port,char* (&recv_buf1), MainWindow *that,int&sockfd)
{   //创建套接字
    Disconnect://若连接断开，则重连
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        perror("socket");
        return -1;
    }
    int optval=1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));
    //目的地址结构体
    cout<<ip<<":"<<port<<endl;
    struct sockaddr_in dst_addr;
    bzero(&dst_addr,sizeof(dst_addr));
    dst_addr.sin_family=AF_INET;
    dst_addr.sin_port=htons(port);
    inet_pton(AF_INET,ip,&dst_addr.sin_addr.s_addr);
    //建立连接 
    int ret=connect(sockfd,(struct sockaddr*)&dst_addr,sizeof(dst_addr));
    if(ret==-1)
    {
        perror("connect");
        return -1;
    }
    char*buf="TCP已连接";
    qDebug()<<"tcpClient sockfd:"<<sockfd;
    that->ui->textBrowser->append(buf);
    send(sockfd,buf,strlen(buf),0);
    char ip_port[30];
    sprintf(ip_port,"%s:%d",ip,port);
    while(1)//开始监听
    {

        memset(recv_buf1,0,256);
        //获取时间
        QDateTime datetime=QDateTime::currentDateTime();
        QString timeStr=datetime.toString("yyyy-MM-dd hh:mm:ss");
        QByteArray ba=timeStr.toLatin1();
        char*time_buf=ba.data();
        sprintf(recv_buf1,"[%s] [%s]:\n",time_buf,ip_port);

        int len=0;
        len=read(sockfd,recv_buf1+strlen(recv_buf1),256-strlen(recv_buf1));
        if(len>0)
        {
            qDebug()<<recv_buf1;
            QString recvbuf(recv_buf1);
            that->ui->textBrowser->append(recvbuf);
        }
        else
        {
            qDebug()<<"TCP连接已断开";
            that->ui->textBrowser->append("TCP连接已断开! 正在尝试重新连接!");
            goto Disconnect;
        }
    }
    return 0;
}

int tcpfd3=-1;//tcpfd3为连接过来的客户端套接字

int tcpServer(char *ip, int port,char* (&recv_buf1), MainWindow *that, int &tcpfd2)
{
    //监听套接字
    tcpfd2=socket(AF_INET,SOCK_STREAM,0);
    if(tcpfd2<0)
    {
        perror("socket");
        return -1;
    }
    int optval=1;
    setsockopt(tcpfd2,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));
    tcpfd2=tcpfd2;
    //绑定端口　ip
    struct sockaddr_in my_addr;
    bzero(&my_addr,sizeof(my_addr));
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(port);
    inet_pton(AF_INET,ip,&my_addr.sin_addr.s_addr);
   // my_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    qDebug()<<"tcpServer:IP:port"<<ip<<port;
    int ret=bind(tcpfd2,(struct sockaddr*)&my_addr,sizeof(my_addr));
    if(ret!=0)
    {
        perror("tcpServer: bind");
        ::close(tcpfd2);
        return -1;
    }
    qDebug()<<"tcpServer:监听套接字tcpfd1:"<<tcpfd2<<"已启动，监听客户端连接";
    //监听连接
    ret=listen(tcpfd2,10);
    if(ret!=0)
    {
        perror("listen:");
        ::close(tcpfd2);
        return -1;
    }
    //accept等待连接并创建客户端结构体
    struct sockaddr_in client_addr;
    socklen_t client_len=sizeof(client_addr);

    while(1)
    {
        qDebug()<<"tcpServer:accept等待可用客户端连接";
        bzero(&client_addr,sizeof(client_addr));
        tcpfd3=accept(tcpfd2,(struct sockaddr*)&client_addr,&client_len);//等待连接
        if(tcpfd3<0)
        {
            perror("accept");
            continue;
            ::close(tcpfd2);
            return -1;
        }
        else
        {
            //存入客户端信息
            unsigned short client_port=ntohs(client_addr.sin_port);
            char client_ip[16]="";
            inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_ip,16);
            char client_buf[20]="";
            sprintf(client_buf,"客户端:%s:%hu 已连接",client_ip,client_port);//存入客户端信息
            QString qsCli(client_buf);
            that->ui->textBrowser->append(qsCli);   //ui上显示客户端连接信息
            send(tcpfd3,"服务器已连接",18,0);

            //容器和下拉框增加已连接的客户端
            char ip_port[128];
            sprintf(ip_port,"%s:%hu",client_ip,client_port);//存入客户端ip：port
            string strIpPort=ip_port;
            that->fdMap.insert(pair<string,int>(strIpPort,tcpfd3));//客户端套接字和ip:port存入map容器
            QString qsip=QString::fromStdString(strIpPort);
            that->ui->comboBox_2->addItem(qsip);    //下拉栏增添新连接的ip:port

            struct ClientMsg cliMsg;
            cliMsg.cli_fd=tcpfd3;
            strcpy(cliMsg.ip_port,ip_port);
            cliMsg.that=that;
            cout<<"tcpServer:cli_fd:"<<cliMsg.cli_fd<<" ip_port:"<<cliMsg.ip_port<<endl;
            pid_t pid=fork();
            if(pid==0)
            {
                close(tcpfd2);
                tcpDealClient(&cliMsg);
                close(tcpfd3);
                qDebug()<<"tcpServer:失连客户端进程即将退出";
                exit(-1);
            }
        }

    }
    return 0;
}
void* SocketConnected(void *cli_msg1)
{
    //解析出参数
    ClientMsg*cli_msg=(ClientMsg*)cli_msg1;
    MainWindow*that=cli_msg->that;
    int sock=cli_msg->cli_fd;
    string ip_port=cli_msg->ip_port;

    char ch_disMsg[100];
    sprintf(ch_disMsg,"客户端：%s 已断开连接",cli_msg->ip_port);
    string disMsg=ch_disMsg;
    QString qsMsg=QString::fromStdString(disMsg);
    QString qsIpPort=QString::fromStdString(ip_port);
    qDebug()<<qsMsg;
    that->ui->textBrowser->append(qsMsg);  //ui显示断开链接信息
    int index=that->ui->comboBox_2->findText(qsIpPort);
    that->ui->comboBox_2->removeItem(index);    //下拉栏删除失连客户端
    that->fdMap.erase(ip_port);     //容器中删除失连客户端

    qDebug()<<"SocketConnect:失连客户端已处理";
    return NULL;
}

void *tcpDealClient(void *arg)//处理tcp服务器连接到的客户端的进程
{
    //获取参数
    ClientMsg*cliMsg=(ClientMsg*)arg;
    MainWindow*that=cliMsg->that;
    int cli_fd=cliMsg->cli_fd;
    string ip_port=cliMsg->ip_port;
    cout<<"tcpDel:客户端进程进行中:"<<ip_port<<endl;

    //开始监听客户端消息
    //pthread_cleanup_push(SocketConnected,arg);//注册线程退出清理函数,我强转了吗？？？！！
    char recv_buf[512];
    int fd=open("myfifo",O_WRONLY); //有名管道，与UＩ进程通信，将接收到的消息传给UＩ
    if(fd<0)
    {
        perror("tcpDel: open");
        return NULL;
    }
    while(1)
    {
        memset(recv_buf,0,512);
        //获取时间
        time_t now_time;
        time(&now_time);
        sprintf(recv_buf,"%s[%s]: ",ctime(&now_time),cliMsg->ip_port);

        cout<<"tcpDel:即将读取:"<<endl;
        int len=read(cli_fd,recv_buf+strlen(recv_buf),512-strlen(recv_buf));
        cout<<"tcpDel:已读取:"<<recv_buf<<endl;

        write(fd,recv_buf,sizeof(recv_buf));
        if(len<=0)
        {
            qDebug()<<"tcpDel:检测到客户端断连";
            SocketConnected(arg);
            break;
        }

    }
    //pthread_cleanup_pop(0);
    qDebug()<<"tcpDel:失连客户端进程即将退出";


    return NULL;

}
