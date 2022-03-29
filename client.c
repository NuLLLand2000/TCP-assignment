<<<<<<< HEAD
//
// Created by Rustylake on 2022/3/25.
//
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/md5.h>


/**
 * 创建套接字
 * @return client_fd
 */
int create_socket() {
    int client_fd = 0;
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket failed");
        return -1;
    }
    return client_fd;
}

char* read_file(char *file_name) {
    FILE *file_ptr;
    char static buff[255];
    file_ptr = fopen(file_name, "r");
    fscanf(file_ptr, "%s", buff);
    fclose(file_ptr);  //关闭文件
    return buff;
}

/**
*  Get MD5 String
*
*/
unsigned char* getMD5(char *input_string) {
    unsigned static char c[MD5_DIGEST_LENGTH];
    char *filename = input_string;
    int i;
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("%s can't be opened.\n", filename);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0) {
        MD5_Update (&mdContext, data, bytes);
    }
    
    MD5_Final (c,&mdContext);
    
    for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
      printf("%02x", c[i]);
    }
    
    printf (" %s\n", filename);
    fclose (inFile);
    return c;
}


int main(int argc, char *argv[]) {
    char *socket_name = argv[1];  //IP地址和端口号
    char *file_name = argv[2];  //文件

    char *ans = read_file(file_name);
    printf("%s", ans);

    int clientfd = create_socket();
    printf("socket ok!\n");
//客户端可以不绑定IP地址和端口号，系统会随机分配
//客户端连接服务器
    int ret = 0;
    int addrlen = 0;
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);  //host to network
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrlen = sizeof(server_address);


    ret = connect(clientfd, (struct sockaddr *) &server_address, addrlen); //连接到服务器
    if (ret < 0) {
        perror("connect failed");
        close(clientfd);
        return -1;
    }
    printf("connect success\n");
//调用send向服务器发送消息
    while (1) {
        int sed = 0;
        int rev = 0;
        char buf[1024] = {0};
        scanf("%s", buf);
        sed = send(clientfd, buf, strlen(buf), 0);
        if (sed < 0) {
            perror("send failed");
            close(clientfd);
            return -1;
        }
        printf("send success\n");

        memset(buf, 0, sizeof(buf));
        rev = recv(clientfd, buf, sizeof(buf), 0);
        if (rev > 0) {
            printf("本次收到了%d个字节\n", rev);
            printf("receive: %s\n", buf);
        }

    }
    close(clientfd);


    return 0;
}


=======

//
// Created by Rustylake on 2022/3/25.
//
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <openssl/md5.h>
#include<stdlib.h>
#include <sys/time.h>
#include <string.h>

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

/**
 * 创建套接字
 * @return client_fd
 */
int create_socket() {
    int client_fd = 0;
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket failed");
        return -1;
    }
    return client_fd;
}

char *read_file(char *file_name) {
    FILE *file_ptr;
    char static buff[102400];
    file_ptr = fopen(file_name, "r");
    fread(buff, 1, 100000, file_ptr);
    fclose(file_ptr);  //关闭文件
    return buff;
}

/**
 *
 * @param c
 * @param input_string
 */
// void getMD5(unsigned char*c,char *input_string) {
//     //unsigned static char c[MD5_DIGEST_LENGTH+1];
//     char *filename = input_string;
//     int i;
//     FILE *inFile = fopen (filename, "rb");
//     MD5_CTX mdContext;
//     int bytes;
//     unsigned char data[1024];

//     if (inFile == NULL) {
//         printf ("%s can't be opened.\n", filename);
//         return ;
//     }

//     MD5_Init (&mdContext);
//     while ((bytes = fread (data, 1, 1024, inFile)) != 0) {
//         MD5_Update (&mdContext, data, bytes);
//     }

//     MD5_Final (c,&mdContext);

// //    for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
// //      printf("%02x", c[i]);
// //    }
//     //printf (" %s\n", filename);
//     fclose (inFile);
// }


//void My_print(unsigned char *ptr) {
//    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
//        printf("%02x", ptr[i]);
//    }
//}




int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "输入IP地址端口号和文件名: %s <HOSTNAME:PORT> <FILENAME>\n", argv[0]);
        exit(1);
    }

    //about time
    struct timeval GTOD_before, GTOD_after, difference;
    char md5[] = "111111111111111";
    char header[1024] = "";
    char *socket_name = argv[1];  //IP地址和端口号
    char *file_name = argv[2];  //文件

    //封装header
    strcat(header, file_name);
    strcat(header, " ");
    strcat(header, md5);
    strcat(header, "\n");
    printf("%s", header);

    //===================到时候恢复
    //unsigned char md5_digest[17];
    //memset(md5_digest,0,sizeof(md5_digest));
    //getMD5(md5_digest, file_name); // digest string
    //My_print(md5_digest);  //打印myprint
    //=====================到时候恢复

    //文件内容
    char *file_contain = read_file(file_name);
    printf("%s\n", file_contain);


    int clientfd = create_socket(); //客户端sock
    printf("socket 建立成功!\n");


    //客户端可以不绑定IP地址和端口号，系统会随机分配
    //客户端连接服务器
    int ret = 0;
    int addrlen = 0;
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);  //host to network
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrlen = sizeof(server_address);


    ret = connect(clientfd, (struct sockaddr *) &server_address, addrlen); //连接到服务器
    if (ret < 0) {
        perror("connect failed");
        close(clientfd);
        return -1;
    }
    printf("connect success\n");


//调用send向服务器发送消息

    int sed = 0;
    int rev = 0;
    char buf[1024] = "发送的内容";

    sed = send(clientfd, buf, strlen(buf), 0);
    if (sed < 0) {
        perror("send failed");
        close(clientfd);
        return -1;
    }
    printf("send success\n");

    memset(buf, 0, sizeof(buf));
    rev = recv(clientfd, buf, sizeof(buf), 0);
    if (rev > 0) {
        printf("Get %d Bytes\n", rev);
        printf("receive: %s\n", buf);
    }
    close(clientfd);
    return 0;
}
>>>>>>> 6ec68ab (完成了fork部分)
