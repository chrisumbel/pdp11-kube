#include <stdio.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h> 
#include <string.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024
int opt = 1; 

int create_sock(struct sockaddr_in bind_addr) {
    int sock;

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Error creating socket");
      exit(1);
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(sock, (struct sockaddr *)&bind_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Error binding to address");
        shutdown(sock, SHUT_RDWR);
        exit(1);
    }

    return sock;
}

int connect_client(int client_sock) {
    struct sockaddr_in simh_addr;

    memset(&simh_addr, 0, sizeof(struct sockaddr_in));
    simh_addr.sin_family = AF_INET;
    simh_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    simh_addr.sin_port = htons(2023);

    while (connect(client_sock, (struct sockaddr*)&simh_addr, sizeof(simh_addr)) < 0) {
        perror("Error connecting to destination");
    }
}

int main() {
    int listen_sock, server_socket, client_sock; 
    struct sockaddr_in server_addr, client_addr; 
    unsigned char buff[BUFF_SIZE];
    int len;
    int addrlen;

    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_addr.sin_port = htons(2083);

    client_sock = create_sock(client_addr);
    connect_client(client_sock);

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(8023); 

    listen_sock = create_sock(server_addr);

    if (listen(listen_sock, 3) < 0) {
        perror("Error listening"); 
        exit(EXIT_FAILURE);
    }

    while(1) {
        printf("====== New Connection ======\n");
        
        if ((server_socket = accept(listen_sock, (struct sockaddr *)&server_addr,  
                        (socklen_t*)&addrlen)) < 0) {
            perror("Errro accepting"); 
            exit(EXIT_FAILURE); 
        } 

        int send_len = 0;

        while(send_len > -1) {
            if((len = recv(server_socket, buff, BUFF_SIZE, MSG_DONTWAIT)) > 0) {
                send(client_sock, buff, len, MSG_DONTWAIT | MSG_NOSIGNAL);
            } else if(len == 0) {
                break;
            }

            if((len = recv(client_sock, buff, BUFF_SIZE, MSG_DONTWAIT)) > 0) {
                send_len = send(server_socket, buff, len, MSG_DONTWAIT | MSG_NOSIGNAL);
            }
        }

        shutdown(server_socket, SHUT_RDWR);
    }

    shutdown(listen_sock, SHUT_RDWR);
    shutdown(client_sock, SHUT_RDWR);

    return 0;
}
