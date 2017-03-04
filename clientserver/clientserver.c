//
//  clientserver.c
//  c_service
//
//  Created by pengge on 17/2/28.
//  Copyright © 2017年 pengge. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//read方法需要的头文件
#include <unistd.h>
//socket方法需要的头文件
#include <sys/socket.h>
#include <sys/types.h>
//htonl 方法需要的头文件
#include <netinet/in.h>
//inet_ntop方法需要的头文件
#include <arpa/inet.h>

#define MAXLINE 100
#define CLI_PORT 8000
//webserver 主程序

int main(int argc, const char * argv[]) {
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    
    int clientfd;
    long n;
    //client socket连接
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    char *str = "hello world";
    
    //sockaddr_in结构体初始化
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(CLI_PORT);
    
    //connect()方法
    connect(clientfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    //write()方法是client 向 server 写数据
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        write(clientfd, buf, strlen(buf));
        printf("write to server : %s\n",buf);
        
        //read()方法是从server接收数据
        n = read(clientfd, buf, strlen(buf));
        if(n == 0) {
            printf("the other side has been close\n");
        }else {
            printf("Response from server: %s\n",buf);
            write(STDOUT_FILENO, buf, n);
            printf("\n");
        }
    }
    close(clientfd);
    
    
    
    
    
}
