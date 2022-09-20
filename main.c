#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 9000

int port = 0;
char *path;
void loadConfig(){
    // file contains format like this:
    /*
        PORT=80
        PATH=/var/www/html/index.html
    */
    FILE *fp;   
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen("config.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1) {
        if(strstr(line, "PORT=") != NULL){
            port = atoi(line + 5);
        }
        if(strstr(line, "PATH=") != NULL){
            path = line + 5;
        }
    }
    fclose(fp);
    printf("File fucking loaded\n");
    printf("Port: %d\n", port);
    printf("Path: %s\n", path);
}
void setUpServer(char httpHeader[]){
    // file is path + index.html
    FILE *fp;
    fp = fopen(strcat(path, "index.html"), "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1) {
        strcat(httpHeader, line);
    }
    fclose(fp);
}

int main(int argc, char const *argv[]){
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Only this line has been changed. Everything is same.
    // give size
    char httpHeader[100000] = "HTTP/1.1 200 OK\r\n\n";
    loadConfig();
    setUpServer(httpHeader);
    printf("Header: %s\n", httpHeader);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("%s\n",buffer );
        write(new_socket , httpHeader , strlen(httpHeader));
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}