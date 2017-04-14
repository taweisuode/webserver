//
//  wrap_socket.c
//  webserver
//
//  Created by pengge on 17/3/1.
//  Copyright © 2017年 pengge. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "wrap_socket.h"

#define MAXLINE 1000
// char[]缓冲区大小
#define _INT_BUF (1024)
// listen监听队列的大小
#define _INT_LIS (7)

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
    if(bind(fd, sa, len) < 0 ) {
        p_error("bind connect error\n");
        exit(1);
    }
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
    size_t  nleft;
    ssize_t nread;
    char   *ptr;
    
    ptr = buf;
    nleft = len;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        } else if (nread == 0)
            break;
        
        nleft -= nread;
        ptr += nread;
    }
    return len - nleft;
}
void Write(int fd, void *buf, size_t len) {
    if(write(fd, buf, len) == -1 )
        p_error("write error\n");
}
void Close(int fd) {
    if(close(fd) == -1)
        p_error("close fd error\n");
}

int Pipe(int pipefd[2])
{
    if(pipe(pipefd)==-1)
    {
        return -1;
    }
    return 1;
}
/*==========================*/
/* 这里是响应请求的封装*/
/*==========================*/
int find_url(char *uri,char *filename,char *cgi_params) {
    char *ptr;
    char temp1[MAXLINE];
    char temp2[MAXLINE];
    //说明走的是静态网址
    if(!strstr(uri, "php")) {
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri)-1] == '/')
            strcat(filename, "index.html");
    }else {
        ptr = index(uri, '?');
        sscanf(uri, "%[^?]?%s",temp1,temp2);
        if(temp2[0] != '\0') {
            strcpy(cgi_params, temp2);
        }
        strcpy(filename, ".");
        strcat(filename, temp1);
    }
    return 1;
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

void php_cgi(char* script_path, int fd,char *cgi_params) {
    char *emptylist[] = {script_path };
    setenv("QUERY_STRING", cgi_params, 1);
    dup2(fd, STDOUT_FILENO);
    //execl("/usr/bin/php","php",script_path,(void *)NULL);
    //execve("./slow-cgi", emptylist, envp);
    execlp("./slow-cgi.cgi",script_path,(char *) NULL);
    //execve(script_path, emptylist, environ);
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
 * @desc 封装get请求response  支持静态页面以及php页面
 *
 */
void wrap_get_response(int connfd,char *filename,char *cgi_params) {
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
            php_cgi(filename, connfd,cgi_params);
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
int getfdline(int fd, char buf[], int sz)
{
    char* tp = buf;
    char c;
    
    --sz;
    while((tp-buf)<sz){
        if(read(fd, &c, 1) <= 0) //伪造结束条件
            break;
        if(c == '\r'){ //全部以\r分割
            if(recv(fd, &c, 1, MSG_PEEK)>0 && c == '\n')
                read(fd, &c, 1);
            else //意外的结束,填充 \n 结束读取
                *tp++ = '\n';
            break;
        }
        *tp++ = c;
    }
    *tp = '\0';
    return tp - buf;
}
ssize_t rio_readnb(int rp, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;
    
    while (nleft > 0) {
        if ((nread = read(rp, bufp, nleft)) < 0) {
            if (errno == EINTR) /* interrupted by sig handler return */
                nread = 0;      /* call read() again */
            else
                return -1;      /* errno set by read() */
        }
        else if (nread == 0)
            break;              /* EOF */
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);         /* return >= 0 */
}
ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;
    
    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR)  /* interrupted by sig handler return */
                nwritten = 0;    /* and call write() again */
            else
                return -1;       /* errorno set by write() */
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}
/*
 * 处理客户端的http请求.
 * cfd		: 客户端文件描述符
 * path		: 请求的文件路径
 * query	: 请求发送的过来的数据, url ? 后面那些数据
 */
void request_cgi(int fd, const char* path, const char* query)
{
    char buf[MAXLINE],data[MAXLINE];
    char contlen_string[MAXLINE];
    int p[2];
    pid_t pid;
    int contlen = -1; //报文长度
    char c;
    while(getfdline(fd, buf, sizeof(buf))>0){
        buf[15] = '\0';
        if(!strcasecmp(buf, "Content-Length:"))
            contlen = atoi(buf + 16);
    }
    if(contlen == -1){ //错误的报文,直接返回错误结果
        p_error("contlen error");
        return;
    }
    
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n",buf);
    sprintf(buf, "%sContent-Type:text/html\r\n",buf);
    
    sprintf(contlen_string, "%d", contlen);
    setenv("CONTENT-LENGTH",contlen_string , 1);
    
    
    read(fd, data, contlen);
    printf("post data= %s\n",data);
    exit(1);
    sprintf(data, "HTTP/1.0 200 OK\r\n");
    sprintf(data, "%sServer: Pengge Web Server\r\n",data);
    sprintf(data, "%sConnection: close\r\n",data);
    sprintf(data, "%sContent-length: %lld\r\n",response,contlen);
    sprintf(data, "%sContent-type: %s\r\n\r\n",data,"");
    Write(connfd, response, strlen(response));
    printf("Response headers:\n");

    /*
    Read(fd, data, contlen);
    //rio_readnb(fd,data,contlen);
    printf("data=%s\n",data);
    printf("contlen=%d\n",contlen);
    exit(1);
    Pipe(p);
    if (fork() == 0)
    {
        rio_readnb(fd,data,contlen);
        rio_writen(p[1],data,contlen);
        //setenv("CONTENT-LENGTH","40" , 1);
        dup2(fd,STDOUT_FILENO);
        execlp("./slow-cgi.cgi", path, (void *) NULL);
    }
    

    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n",buf);
    setenv("CONTENT-LENGTH",contlen , 1);
    
    rio_writen(fd, buf, strlen(buf));
    
    //Wait(NULL);
    dup2(p[0],STDIN_FILENO);
    Close(p[0]);
    
    Close(p[1]);

    //dup2(fd,STDOUT_FILENO);
    //execlp("./slow-cgi.cgi", path, (void *) NULL);
     */
}
