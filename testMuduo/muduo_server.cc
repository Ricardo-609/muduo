#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <functional>
#include <iostream>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

//基于Muduo网络库开发服务器程序
/*
1. 组合TcpServer对象
2. 创建EventLoop事件循环对象的指针
3. 明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数\
4. 在当前服务器类的构造函数中，注册处理连接和处理读写事件的回调函数
5. 设置合适的服务端线程数量，muduo库会自己划分IO线程和worker线程
*/
//重点部分在onConnection和onMessage，其他部分由网络库来完成
class ChatServer
{
public:
    //参数依次为：事件循环 IP+Port 服务器的名字
    ChatServer(EventLoop *loop, const InetAddress &listenAddr,
               const string &nameArg)
        : server_(loop, listenAddr, nameArg), loop_(loop)
    {
        //给服务器注册用户连接的创建和断开回调
        server_.setConnectionCallback(
            std::bind(&ChatServer::onConnection, this, _1));

        //给服务器注册用户读写事件回调
        server_.setMessageCallback(
            std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        //设置服务器端的线程数量 1个IO线程 3个worker线程
        server_.setThreadNum(4);
    }

    //开启事件循环
    void start() { server_.start(); }

private:
    //专门处理用户的连接创建和断开  epoll listenfd accept
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << " -> "
                 << conn->localAddress().toIpPort() << " state:online" << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << " -> "
                 << conn->localAddress().toIpPort() << " state:offline" << endl;
            conn->shutdown(); // close(fd)
                              // loop_->quit();  //退出服务器
        }
    }

    //专门处理用户的读写事件。参数：连接 缓冲区 接收到数据的时间信息
    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data: " << buf << " time : " << time.toString() << endl;
        conn->send(buf);
    }

    TcpServer server_; //#1。TcpServer没有默认构造，所以需要这个
    EventLoop *loop_;  //#2    epoll
};

int main()
{
    EventLoop loop; // epoll
    InetAddress addr("127.0.0.1", 8000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start(); //启动服务:  listenfd epoll_ctl -> epoll
    loop.loop();    // epoll_wait 以阻塞的方式等待新用户连接 或已连接用户的读写事件等

    return 0;
}