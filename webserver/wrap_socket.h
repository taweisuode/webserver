//
//  wrap_socket.h
//  webserver
//
//  Created by pengge on 17/3/1.
//  Copyright © 2017年 pengge. All rights reserved.
//

#ifndef wrap_socket_h
#define wrap_socket_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//read方法需要的头文件
#include <unistd.h>
//socket方法需要的头文件
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
//htonl 方法需要的头文件
#include <netinet/in.h>
//inet_ntop方法需要的头文件
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>

int Socket(int family,int type,int protocol);
int num_sum(int a,int b);
void Bind(int fd, const struct sockaddr *sa, socklen_t len);
void Listen(int fd,int backlog_size);
int Accept(int fd,const struct sockaddr *sa,socklen_t *len);
void Connect(int fd,const struct sockaddr *sa,socklen_t len);
long Read(int fd, void *buf, size_t len);
void Write(int fd, void *buf, size_t len);
void Close(int fd);
int find_url(char *uri,char *filename,char *cgi_params);
void wrap_get_response(int connfd,char *filename,char *cgi_params);
void wrap_post_response(int connfd,char *filename,char *cgi_params);
void request_cgi(int cfd, const char* path, const char* query);
//void post_dynamic(int fd, char *filename, int contentLength);

#endif /* wrap_socket_h */

