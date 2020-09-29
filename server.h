#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include "init.h"

// ostatni pakiet może mieć inną długość
int get_packet_size(int i)
{
    int off = i * PACKET_SIZE;
    return off + PACKET_SIZE > total_download ? total_download - off : PACKET_SIZE; 
}

int setup_server()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(54321);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

// wyślij żądanie o i-ty pakiet
void send_request(int sockfd, int i)
{
    int start = i * PACKET_SIZE, size = get_packet_size(i);
    if (start + PACKET_SIZE > total_download)
        size = total_download - start;

    static char msg[20];
    sprintf(msg, "GET %d %d\n", start, size);
    
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
    server_address.sin_addr.s_addr = ip;
    
    ssize_t msg_len = strlen(msg);
    if (sendto(sockfd, msg, msg_len, 0, (struct sockaddr*)&server_address, sizeof(server_address)) != msg_len)
    {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

// przyjmij pakiet, wywoływana, gdy recvfrom jest nieblokujące
int receive_packet(int sockfd, char** it)
{
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    static char buffer[BUFFER_SIZE];

    ssize_t datagram_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&sender, &sender_len);
    if (datagram_len < 0)
    {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (sender.sin_addr.s_addr != ip || sender.sin_port != port)
        return -1;

    *it = buffer;
    return 1;
}