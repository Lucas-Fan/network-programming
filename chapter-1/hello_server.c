#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock; // server socket
    int clnt_sock; // client socket

    struct sockaddr_in serv_addr; // 主套接字，用于持续监听连接请求
    struct sockaddr_in clnt_addr; // 辅助套接字：用于与客户端相连接以传输数据
    socklen_t clnt_addr_size;

    char message[] = "Hello World!";

    if (argc != 2) // 两个参数：可执行文件名、端口号
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0); // 创建了一个 TCP 套接字
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));      // 将 serv_addr 全部填充为 0，主要是为了将 serv_addr 的 sin_zero 成员设为 0
    serv_addr.sin_family = AF_INET;                // 选定 IPv4 地址族
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // htonl：将 long 类型数据从主机字节序转换为网络字节序; INADDR_ANY：32 位整型值表示的 IP 地址
    serv_addr.sin_port = htons(atoi(argv[1]));     // 此程序运行时应该在文件名后跟一个端口号作为参数，如 hello_server 3030

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) // 将套接字与服务器的 IP 地址和端口号相绑定
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1) // 将套接字转换为接听状态
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size); // 接受一个连接请求，并将 clnt_sock 套接字与其相连接
    if (clnt_sock == -1)
        error_handling("accept() error");

    write(clnt_sock, message, sizeof(message)); // 向客户端发送信息。注意：clnt_sock 不是客户端的套接字，而是服务器上真正与客户端相连接的套接字
    close(clnt_sock);                           // 关闭与客户连接的套接字：断开了该连接
    close(serv_sock);                           // 关闭监听端口的套接字：不再接受任何请求
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}