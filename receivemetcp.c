#include<stdio.h>
#include<sys/types.h>//socket
#include<sys/socket.h>//socket
#include<string.h>//memset
#include<stdlib.h>//sizeof
#include<netinet/in.h>//INADDR_ANY
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <openssl/md5.h>
#include <sys/wait.h>

#define SEND_AND_RECEIVE_LENGTH 274

/*MD5函数*/
char * calculate_file_md5(const char *filename) {
	unsigned char c[MD5_DIGEST_LENGTH];
	int i;
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];
	char *filemd5 = (char*) malloc(33 *sizeof(char));

	FILE *inFile = fopen (filename, "rb");
	if (inFile == NULL) {
		perror(filename);
		return 0;
	}

	MD5_Init (&mdContext);

	while ((bytes = fread (data, 1, 1024, inFile)) != 0)

	MD5_Update (&mdContext, data, bytes);

	MD5_Final (c,&mdContext);

	for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(&filemd5[i*2], "%02x", (unsigned int)c[i]);
	}

	//printf("calculated md5:%s ", filemd5);
	//printf (" %s\n", filename);
	fclose (inFile);
	return filemd5;
}

/**
 * 进程处理，等待函数
 * @param signal
 */
void handler(int signal) {
    wait(NULL);
}

int childCnt;

/**
 * 创建服务器socket
 * @param port 端口号
 * @return int 服务器socket的标识符
 */
int create_server_socket(int port) {
    int length;
    int server_sock_fd;
//创建socket
    server_sock_fd = socket(PF_INET, SOCK_STREAM, 0);  //创建socket，返回唯一标识符，0为参数使用默认协议
    if (server_sock_fd == -1) {
        perror("socket fail");
        return EXIT_SUCCESS;
    }

    //设置允许重复使用本机地址，设置端口复用
    int yes = 1;
    if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("set socket option error\n");
    }

// 填充服务器网络信息结构体
    length = sizeof(struct sockaddr_in);
    struct sockaddr_in server_addr;
    bzero(&server_addr, length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);  //主机到网络的转换 监听的端口
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //本机地址，监听socket
//    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //本机地址，监听socket

    //======================绑定
    int bind_res = bind(server_sock_fd, (struct sockaddr *) &server_addr, length);
    if (bind_res == -1) { //绑定失败
        perror("bind fail");
        return EXIT_FAILURE;
    }
    return server_sock_fd;
}

/* 
	Calculate the difference between to timevals; store result in an timeval. 
	syntax: a,b,c.
	Result: c=a-b;
*/
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


void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}


//Convert a struct sockaddr address to a string, IPv4 and IPv6:
char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen) {
    switch (sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *) sa)->sin_addr),
                      s, maxlen);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) sa)->sin6_addr),
                      s, maxlen);
            break;

        default:
            strncpy(s, "Unknown AF", maxlen);
            return NULL;
    }

    return s;
}

/**
 * timeval的结构体转化为double
 * @param time_value timeval变量
 * @return double变量
 */
double time_2_dbl(struct timeval time_value) {
    double new_time = 0;
    new_time = (double) (time_value.tv_usec);
    new_time /= 1000000;
    new_time += (double) time_value.tv_sec;
    return (new_time);
}

static const char *program_name;

int main(int argc, char *argv[]) {
    int listenfd;  //to create socket
    int accept_fd;  //to accept connection
    childCnt = 0;// Will increment for each spawned child.

    struct timeval GTOD_before, GTOD_after, difference; //用来处理时间

    struct addrinfo hints, *servinfo, *p;
    int rv;

    struct sockaddr_in serverAddress;//server receive on this address
    struct sockaddr_in clientAddress;//server sends to client on this address
    struct sockaddr_storage their_addr;

    int n;
    char msg[1450];
    char cli[INET6_ADDRSTRLEN];
    int clientAddressLength;
    int pid;

    const char *separator = strrchr(argv[0], '/');
    if (separator) {
        program_name = separator + 1;
    } else {
        program_name = argv[0];
    }
// ================以上为外教编写

    // Do something
    if (argc != 2) {
        printf("请输入端口号 %s <PORT>\n", argv[0]);
        return 0;
    }
    int port = atoi(argv[1]); //端口号

    //================创建socket，绑定socket
    listenfd = create_server_socket(port);


    //=================被动监听
    int listen_res = listen(listenfd, 5);  //最多允许5台主机接入
    if (listen_res == -1) {  //监听失败
        perror("fail to listen");
        exit(1);
    }

    //使用信号，异步处理僵尸进程
    signal(SIGCHLD, handler);

    socklen_t address_len = sizeof(clientAddress);

    while (1) {
        //long t = time(NULL);
        //阻塞等待连接请求
        accept_fd = accept(listenfd, (struct sockaddr *) &clientAddress, &address_len);
        if (accept_fd == -1) {
            return 0;
        }
        gettimeofday(&GTOD_before, NULL);  //连接成功的时间
        //连接的客户端信息
        printf("ip:%s, port:%d\n",
               inet_ntoa(clientAddress.sin_addr),
               ntohs(clientAddress.sin_port)
        );

        //fork 创建子进程，父亲继续连接，子和客户端通信
        pid_t pid;
        if ((pid = fork()) < 0) {
            perror("进程创建错误\n");
            exit(1);
        } else if (pid > 0) {
            //父亲accept，阻塞
        } else {
            //子通信
            //服务器接收
            char receive_header[SEND_AND_RECEIVE_LENGTH] = "";
            ssize_t bytes;

            if ((bytes = recv(accept_fd, receive_header, SEND_AND_RECEIVE_LENGTH, 0)) == -1) {
                perror("接收信息失败");
            } else if (bytes == 0) {
                printf("客户端退出\n");
            }
            printf("来自客户端的消息：%s\n", receive_header);

            //服务器发送信息
            int send_res = send(accept_fd, "ok\n", SEND_AND_RECEIVE_LENGTH, 0);
            if (send_res == -1) {
                perror("服务器发送失败\n");
                exit(1);
            }

            //再次接受信息
            char receive_file[SEND_AND_RECEIVE_LENGTH] = "";
            ssize_t rece_stat2 = recv(accept_fd, receive_file, SEND_AND_RECEIVE_LENGTH, 0);
            if (rece_stat2 == -1) {
                perror("文件内容接收失败\n");
            }
            printf("收到的文件内容为:\n%s\n", receive_file);

            gettimeofday(&GTOD_after, NULL);  //结束时间

            timeval_subtract(&difference, &GTOD_after, &GTOD_before);  //传输时间差
            close(accept_fd);
            printf("客户端已断开连接\n");

            char *pch = strtok(receive_header, " "); //split header with " "
            char * predefined_md5 = strtok(NULL, " ");
            //写入文件
            char file_name[260] = "received_files/";
            strcat(file_name, pch);
            int res = system("mkdir received_files");
            FILE *fp;
            fp = fopen (file_name, "w+");

            for (int i = 0; i < strlen(receive_file); i++) {
                fputc(receive_file[i], fp);
            }
            fclose (fp);
            
            //校验md5hash
            char * rece_md5 = calculate_file_md5("received_files/a.txt");
            char *md5_stat = "hhhhhhh";
            

            printf("%s\n", predefined_md5);

            if (strcmp(predefined_md5, rece_md5)) {
                md5_stat = "MD5 matched\n";
            } else {
                md5_stat = "md5 not matched\n";
            }

            printf("%f | %s:%u | %s | %ld | %e | %f[s] | %s\n",
                   time_2_dbl(GTOD_before),  //连接时间
                   inet_ntoa(clientAddress.sin_addr), //客户端地址与端口
                   ntohs(clientAddress.sin_port),
                   pch, //文件名
                   strlen(receive_file), //传输大小 %d
                   (sizeof(receive_file) * 8) / time_2_dbl(difference),  //传输速率 %e
                   time_2_dbl(difference), //持续时间
                   md5_stat //md5是否匹配
            );

        }
    }
    return 0;
}
