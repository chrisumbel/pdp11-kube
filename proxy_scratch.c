#include <stdio.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h> 
#include <string.h>
#include <arpa/inet.h>

int main() {
    int server_fd, server_socket, client_sock; 
    struct sockaddr_in server_addr, client_addr, simh_addr; 
    unsigned char buff[1024];
    int len;
    int addrlen;






    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_addr.sin_port = htons(2092);

    memset(&simh_addr, 0, sizeof(struct sockaddr_in));
    simh_addr.sin_family = AF_INET;
    simh_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    simh_addr.sin_port = htons(2023);    

    if (client_sock < 0) {
      perror("ERROR opening socket");
      exit(1);
    }

    int opt = 1; 
    if (setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    if (bind(client_sock, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Bind to Port Number");
        shutdown(client_sock, SHUT_RDWR);
        exit(1);
    }

    while (connect(client_sock, (struct sockaddr*)&simh_addr, sizeof(simh_addr)) < 0) {
        perror("ERROR connecting");
    }








    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("Socket failed"); 
        exit(EXIT_FAILURE); 
    }     
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(8023); 

    if (bind(server_fd, (struct sockaddr *)&server_addr,  
                                 sizeof(server_addr))<0) 
    { 
        perror("Bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    if (listen(server_fd, 3) < 0) {
        perror("Listen"); 
        exit(EXIT_FAILURE);
    }

    while(1) 
    {
        printf("====== New Connection ======\n");
        if ((server_socket = accept(server_fd, (struct sockaddr *)&server_addr,  
                        (socklen_t*)&addrlen)) < 0)
        {
            perror("Server Accept"); 
            //exit(EXIT_FAILURE); 
        } 

        int send_len = 0;

        while(send_len > -1) 
        {
            if((len = recv(server_socket, buff, 1024, MSG_DONTWAIT)) > 0)
            {
                printf("OUTGOING --------------------------------------------\n");
                printf("%d\n", len);            
                printf("%.*s\n", len, buff);

                send(client_sock, buff, len, MSG_DONTWAIT | MSG_NOSIGNAL);
            } else if(len == 0) {
                printf("breaking\n");
                break;
            }

            if((len = recv(client_sock, buff, 1024, MSG_DONTWAIT)) > 0)
            {
                printf("INCOMING --------------------------------------------\n");
                printf("%d\n", len);            
                printf("%.*s\n", len, buff);

                send_len = send(server_socket, buff, len, MSG_DONTWAIT | MSG_NOSIGNAL);
            }
        }

        //shutdown(server_socket, SHUT_RDWR);
    }

    shutdown(server_fd, SHUT_RDWR);
    shutdown(client_sock, SHUT_RDWR);

    return 0;
}