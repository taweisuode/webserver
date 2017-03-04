//
//  wrap_socket.c
//  webserver
//
//  Created by pengge on 17/3/1.
//  Copyright © 2017年 pengge. All rights reserved.
//

#include "wrap_socket.h"

#define MAXLINE 1000

void p_error(char *str) {
    printf("%s\n",str);
}
/*==========================*/
/* 这里是所有socket方法的封装*/
/*==========================*/
int Socket(int family,int type,int protocol) {
    int socketfd = socket(family, type, protocol);
    
    if(socketfd < 0) {
        p_error("socket connect error\n");
        return -1;
    }
    return socketfd;
}

void Bind(int fd, const struct sockaddr *sa, socklen_t len) {
    if(bind(fd, sa, len) < 0 )
        p_error("bind connect error\n");
}
void Listen(int fd,int backlog_size) {
    if(listen(fd, backlog_size) < 0)
        p_error("listen client error\n");
}
int Accept(int fd,const struct sockaddr *sa,socklen_t *len) {
    int clientfd = accept(fd, sa, len);
    if(clientfd < 0)
        p_error("can't accept clientserver\n");
    return clientfd;
}
void Connect(int fd,const struct sockaddr *sa,socklen_t len) {
    if(connect(fd, sa, len) < 0)
       p_error("connect to webserver error\n");
}
long Read(int fd, void *buf, size_t len) {
    long n;
    if((n = read(fd, buf, len)) == -1)
        p_error("read error\n");
    return n;
}
void Write(int fd, void *buf, size_t len) {
    if(write(fd, buf, len) == -1 )
        p_error("write error\n");
}
void Close(int fd) {
    if(close(fd) == -1)
        p_error("close fd error\n");
}

/*==========================*/
/* 这里是响应请求的封装*/
/*==========================*/
int find_url(char *uri,char *filename) {
    char *ptr;
    //说明走的是静态网址
    if(!strstr(uri, "cgi-bin")) {
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri)-1] == '/')
            strcat(filename, "index.html");
        return 1;
    }else {
        ptr = index(uri, '?');
        strcpy(filename, ".");
        strcat(filename, uri);
        return 0;
    }
}
/**
 * @desc 获取静态文件类型
 *
 */
void get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html") || strstr(filename, ".php"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else
        strcpy(filetype, "text/plain");
}

void php_cgi(char* script_path, int fd) {
    dup2(fd, STDOUT_FILENO);
    execl("/usr/bin/php","/usr/bin/php",script_path,(char *) NULL);
}
/**
 * @desc 404页面的response拼接
 *
 */
void error_response(int connfd) {
    struct stat sbuf;
    int filefd;
    char *srcp;
    char error_body[MAXLINE],response[MAXLINE];
    char filename[] = "./404.html";
    
    stat(filename,&sbuf);
    filefd = open(filename,O_RDONLY);
    
    sprintf(response, "HTTP/1.1 404 Not found\r\n");
    sprintf(response, "%sServer: Pengge Web Server\r\n",response);
    sprintf(response, "%sConnection: close\r\n",response);
    sprintf(response, "%sContent-length: %lld\r\n",response,sbuf.st_size);
    sprintf(response, "%sContent-type: text/html\r\n\r\n",response);
    Write(connfd, response, strlen(response));
    printf("Response headers:\n");
    srcp = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, filefd, 0);
    Close(filefd);
    //清空srcp空间
    Write(connfd, srcp, sbuf.st_size);
    munmap(srcp, sbuf.st_size);

}
/**
 * @desc 封装response  支持静态页面以及php页面
 *
 */
void wrap_response(int connfd,char *filename) {
    struct stat sbuf;
    int filefd,phpfd;
    char *php_result;
    char *srcp;
    char response[MAXLINE],filetype[MAXLINE];
    if(stat(filename,&sbuf) < 0) {
        error_response(connfd);
        exit(1);
    }else {
        
        //获取文件类型
        get_filetype(filename,filetype);
        
        //打开文件并将其写入内存，并由浏览器展示
        filefd = open(filename,O_RDONLY);
        //走php脚本执行输出
        if(strstr(filename, ".php")) {
            sprintf(response, "HTTP/1.1 200 OK\r\n");
            sprintf(response, "%sServer: Pengge Web Server\r\n",response);
            sprintf(response, "%sConnection: close\r\n",response);
            sprintf(response, "%sContent-type: %s\r\n\r\n",response,filetype);
            Write(connfd, response, strlen(response));
            printf("Response headers:\n");
            printf("%s\n",response);
            php_cgi(filename, connfd);
            Close(connfd);
            exit(1);
        //走静态页面输出
        }else {
            
            //拼接静态文件的response头
            sprintf(response, "HTTP/1.0 200 OK\r\n");
            sprintf(response, "%sServer: Pengge Web Server\r\n",response);
            sprintf(response, "%sConnection: close\r\n",response);
            sprintf(response, "%sContent-length: %lld\r\n",response,sbuf.st_size);
            sprintf(response, "%sContent-type: %s\r\n\r\n",response,filetype);
            Write(connfd, response, strlen(response));
            printf("Response headers:\n");
            printf("%s\n",response);
            srcp = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, filefd, 0);
            Close(filefd);
            //清空srcp空间
            Write(connfd, srcp, sbuf.st_size);
            munmap(srcp, sbuf.st_size);
        }
    }
}
