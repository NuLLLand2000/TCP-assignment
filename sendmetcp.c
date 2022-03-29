#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#define SEND_AND_RECEIVE_LENGTH 274

/* Subtract the ‘struct timeval’ values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y) {
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}


int main(int argc, char *argv[]) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes, numread;
    char buffer[1450];

    struct timeval GTOD_before, GTOD_after, difference;

    char **tokens;
    FILE *fptr;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <HOSTNAME:PORT> <FILENAME>\n", argv[0]);
        exit(1);
    }
    char delim[] = ":";
    char *Desthost = strtok(argv[1], delim);   //IP地址
    char *Destport = strtok(NULL, delim);  //端口号
    char *filename = argv[2];

    printf("Opening %s sending to %s:%s \n", filename, Desthost, Destport);

//Do something ==============以上为外教编写内容


//============================计算md5
    char md5[] = "334bf81dc9377c2c0bab14ffa6f43bcb";  //假设计算完成
    char header[1024] = ""; //定义header


    //header拼接，计算header长度
    strcat(header, filename);
    strcat(header, " ");
    strcat(header, md5);
    strcat(header, "\n");

    printf("%s", header);

//============================创建客户端socket
    sockfd = socket(PF_INET, SOCK_STREAM, 0);  //返回文件描述符
    if (sockfd == -1) { //如果创建失败
        perror("fail to socket");
        exit(1);
    }

// =======================信息结构体
    int sock_addr_length = sizeof(struct sockaddr_in);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(Desthost);  //ZHH ADD 定义主机
    server_addr.sin_port = htons(atoi(Destport));  //定义端口

    if (inet_pton(AF_INET, Desthost, &server_addr.sin_addr) == 0) { //判断IP是否合法
        printf("Invalid IP adress 非法IP\n");
        return EXIT_FAILURE;
    }

//==========================发送客户端连接请求
    int connect_res = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));

    if (connect_res == -1) {  //如果连接失败
        perror("fail to connect");
        exit(1);
    }
    printf("成功连接到服务器，连接代码为 %d，准备发送数据！\n", connect_res);

    gettimeofday(&GTOD_before, NULL);  //计算发送前的时间


// Do something

//强哥内容
/**
    sendto(
            sockfd,
            header, //发送内容
            1000,
            0,
            (struct sockaddr *) &server_addr,
            sock_addr_length
    );
*/
//=========================进行通信 发送数据 write()
    while (1) {
        int client_send_res = send(  //发送
                sockfd,
                header,  //发送内容
                SEND_AND_RECEIVE_LENGTH,  //发送内容的长度 256+1+16+1
                0 //阻塞
        );
        if (client_send_res == -1) {  //发送失败
            perror("fail to send");
            exit(1);
        }
        printf("数据发送成功, 准备接受数据\n");

        //=========================接受服务器的数据 read()

        char rece_msg[SEND_AND_RECEIVE_LENGTH] = "";
        int rece_res = recv(
                sockfd,
                rece_msg,  //收到的内容
                SEND_AND_RECEIVE_LENGTH,
                0
        );
        if (rece_res == -1) { //接受失败
            perror("fail to recv");
            exit(1);
        }
        printf("来自服务器：%s\n", rece_msg);
    }
    int client_send_res = send(  //发送
        sockfd,
        header,  //发送内容
        SEND_AND_RECEIVE_LENGTH,  //发送内容的长度 256+1+16+1
        0 //阻塞
    );
    if (client_send_res == -1) {  //发送失败
        perror("fail to send");
        exit(1);
    }
    printf("数据发送成功, 准备接受数据\n");

//=========================接受服务器的数据 read()

    char rece_msg[SEND_AND_RECEIVE_LENGTH] = "";
    int rece_res = recv(
            sockfd,
            rece_msg,  //收到的内容
            SEND_AND_RECEIVE_LENGTH,
            0
            );
    if (rece_res == -1) { //接受失败
        perror("fail to recv");
        exit(1);
    }
    printf("来自服务器：%s\n", rece_msg);

    close(sockfd); //关闭连接
//强哥内容
/**
    fptr = fopen(filename, "r");  //打开文件

    int cnt = 0;
    for (;;) {
        memset(buffer, 0, sizeof(buffer));
        int f_code = fread(buffer, 1, 1000, (FILE *) fptr);
        if (f_code == 0) break;
        cnt += f_code;
        int m = sendto(sockfd, buffer, 1000, 0, (struct sockaddr *) &server_addr, sock_addr_length);
        if (m == -1)return 0;
    }
    printf(", Sent %d bytes,in", cnt);
*/
    gettimeofday(&GTOD_after, NULL);

    timeval_subtract(&difference, &GTOD_after, &GTOD_before);  //传输时间差

    if (difference.tv_sec > 0) {
        printf("%ld.%06d [s]\n", difference.tv_sec, difference.tv_usec);
    } else {
        printf("%6d [us]\n", difference.tv_usec);
    }

    return 0;
}
