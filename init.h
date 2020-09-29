#ifndef INIT
#define INIT

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

// wielkość fragmentu (pakietu), który żądamy
#define PACKET_SIZE 1000
// wielkość bufora, przy pobieraniu pakietu UDP
#define BUFFER_SIZE 1100

// ip serwera
uint32_t ip;
// port serwera
uint16_t port;
// plik wyjściowy
FILE* output;
// liczba bajtów do pobrania
int total_download;

void format_error(char* msg)
{
    fprintf(stderr, "Error parsing command line arguments. %s\n", msg);
    fprintf(stderr, "Usage: ./transport ip port output_file bytes\n");
}

void parse_args(int argc, char* argv[])
{
    if (argc != 5)
    {
        format_error("");
        exit(EXIT_FAILURE);
    }
    if (inet_pton(AF_INET, argv[1], &ip) <= 0)
    {
        format_error("Invalid IP address.");
        exit(EXIT_FAILURE);
    }
    port = htons(atoi(argv[2]));
    output = fopen(argv[3], "w");
    if (output == NULL)
    {
        fprintf(stderr, "output file error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    total_download = atoi(argv[4]);
}

#endif