//
//  read_config.c
//  webserver
//
//  Created by pengge on 17/3/2.
//  Copyright © 2017年 pengge. All rights reserved.
//

#include "read_config.h"
#define MAXLINE 1000
char* split(char *sep,char *content) {

    char *p;
    p=strtok(content,"=");
    while (p != NULL) {
        printf("p======%s\n",p);
        p=strtok(NULL,"\n");
        printf("p======%s\n",p);
        p=strtok(NULL,"=");
    }
    return p;
    /*
    char *temp,*port;
    temp = strtok(content, sep);
    printf("temp=%s\n",temp);
    port = strtok(temp, "\n");
    return port;
    */
}
void read_config() {
    char  *now_dic,*port,*dir_file;
    char content[MAXLINE];
    FILE *filefd;
    char ch;
    int s = 0;
    now_dic ="/Users/pengge/Desktop/languageStudy/c/webserver/webserver/myhttp.conf";
    filefd = fopen(now_dic, "r+");
    if(filefd == NULL) {
        printf("open file fail\n");exit(1);
    }
    while ((ch=fgetc(filefd))!=EOF)
    {
        content[s] = ch;
        //putchar(ch);
        s++;
    }
    printf("content=%s\n",content);
    if(strstr(content,"Port=")) {
        char *myArray[2];
        memset(myArray, 0x0, sizeof(myArray));
        port = split("Port=",content);

        printf("port=%s\n",port);
    }
    
    //printf("port :%s\n",port);

}
