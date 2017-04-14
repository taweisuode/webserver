//
//  main.c
//  c_service
//
//  Created by pengge on 17/2/20.
//  Copyright © 2017年 pengge. All rights reserved.
//

#include "wrap_socket.h"
#include "read_config.h"

#define MAXLINE 80
#define SERV_PORT 8000
#define BACKLOGSIZE 20
char* str_revert(char *str) {
    long count = strlen(str);
    char temp;
    for (int i = 0; i < count/2; ++i) {
        temp = str[i];
        str[i] = str[count-i-1];
        str[count-i-1] = temp;
    }
    return str;
}
void p_version() {
    printf("pengge websever 1.0.0(built: April 13 2016 15:13:00)\n");
    exit(1);
}
void p_help() {
    printf("Usage\n");
    printf("    -v          show webserver version\n");
    printf("    --version   show webserver version\n");
    printf("    -h          show webserver help list\n");
    printf("    --help      show webserver help list\n");
    exit(1);
}

/**
 * @desc webserver 主程序
 *
 */
int main(int argc, char * argv[]) {
    
    if(argv[1]) {
        if(strcasecmp(argv[1],"-v") == 0 || strcasecmp(argv[1], "--version") == 0) {
            p_version();
        }else if(strcasecmp(argv[1], "-h") == 0 || strcasecmp(argv[1], "--help") == 0) {
            p_help();
        }
    }
    //server 端依次进行 socket(),bind(),listen(),然后进入阻塞等待client连接方法accept()
    struct sockaddr_in servaddr,cliaddr;
    socklen_t cliaddr_len;
    int listenfd,connfd;
    char buf[MAXLINE],first_line[MAXLINE],left_line[MAXLINE],method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char str[INET_ADDRSTRLEN];
    char filename[MAXLINE],cgi_params[MAXLINE];
    long n;
    int i,pid;
    
    //读取文件配置
    //read_config();
    
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    //初始化myaddr参数
    bzero(&servaddr, sizeof(servaddr)); //结构体清零
    //对servaddr 结构体进行赋值
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    Listen(listenfd, BACKLOGSIZE);
    printf("Accepting Connections ...\n");
    
    //死循环中进行accept()
    while (1) {
        cliaddr_len = sizeof(cliaddr);
        
        //accept()函数返回一个connfd描述符
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
        //fork()方法 创建一个跟父进程一摸一样的进程 pid <0 表示fork失败 pid＝＝0表示为子进程 pid>0 为父进程 其pid=getpid();
        pid = fork();
        if(pid < 0) {
            printf("fork error\n");exit(1);
        }else if(pid == 0) {
            while (1) {
                n = Read(connfd, buf, MAXLINE);
                if (n == 0) {
                    printf("the other side has been closed.\n");
                    break;
                }
                //printf("received from %s at PORT %d,message is %s\n",inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),ntohs(cliaddr.sin_port),buf);
                //解析来自浏览器的buf
                sscanf(buf, "%s %s %s",method,uri,version);
                printf("method:%s\n",method);
                printf("uri:%s\n",uri);
                printf("version:%s\n",version);
                //这块先支持get请求
                
                if(strcasecmp(method, "GET") == 0 && strstr(version,"HTTP")) {
                    if(find_url(uri,filename,cgi_params)) {
                        wrap_get_response(connfd,filename,cgi_params);
                    }
                    
                }else if(strcasecmp(method, "POST") == 0 && strstr(version,"HTTP")) {
                    if(find_url(uri,filename,cgi_params)) {
                        request_cgi(connfd,filename,cgi_params);
                    }
                }
            }
            Close(connfd);
            exit(0);
        }else {
            Close(connfd);
            continue;
        }

    }
}
