//
//  slow-cgi.c
//  webserver
//
//  Created by pengge on 17/3/14.
//  Copyright © 2017年 pengge. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <syslog.h>
#include <pwd.h>
#include <grp.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAXLINE 1024

int main(int argc, char * argv[]) {
    char *cgi_params,*script_path;
    int post_content_length;
    char content[MAXLINE],data[MAXLINE];
    
    printf("Content-type: text/html\r\n\r\n");
    //获取get数据
    cgi_params = getenv("QUERY_STRING");
    
    if(getenv("CONTENT-LENGTH") != NULL) {
        //获取post长度
        post_content_length = atol(getenv("CONTENT-LENGTH"));
        printf("post_content_length=%d\n",post_content_length);
        
        //获取缓冲区内容，也就是获取post内容
        fflush(stdin);
        while((fgets(data,post_content_length+1,stdin)) != NULL) {
            sprintf(content, "Info:%s\r\n",data);
            printf("Content-length: %lu\r\n", strlen(content));
            printf("Content-type: text/html\r\n\r\n");
            printf("%s", content);
            exit(1);
        }
        fflush(stdout);
        exit(0);
        exit(1);
        script_path = argv[0];
    }
    script_path = argv[0];
    execl("/usr/local/php56/bin/php-cgi","php-cgi",script_path,cgi_params,(void *)NULL);
    exit(1);
}
