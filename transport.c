#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "init.h"
#include "server.h"
#include "segment.h"

// początek ramki, koniec ramki, wielkość ramki
int lar, lars, sws;
int total_packet_cnt;

// wyślij żądanie o i-ty packiet times razy
void request(int sockfd, int i, int times)
{
    szero(segments + get_offset(i));
    while (times--)
        send_request(sockfd, i);
}

void output_to_file(char* data)
{
    fwrite(data, sizeof(char), 10, output);
}

// czekanie na pakiety przez pewien czas
void receive_packets(int sockfd)
{
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    
    struct timeval tv = {0, 10000};
    while (tv.tv_sec > 0 || tv.tv_usec > 0)
    {
        int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
        if (ready <= 0)
            return;

        char* buffer;
        if (receive_packet(sockfd, &buffer) > 0)
            if (add_new_segment(buffer, lar)) 
                printf("Done %.2f%%\n", 100.f * recv_len / total_download);

        if (recv_len == total_download)
            break;
    }
}

// przesuń obecną ramkę
void advance_frame(int sockfd)
{
    int offset = get_offset(lar);
    while (lar < total_packet_cnt && segments[offset].received)
    {
        fwrite(segments[offset].data, sizeof(char), get_packet_size(lar++), output);
        --recv_frame;
        if (++offset == MAX_SWS)
            offset = 0;
        if (lars < total_packet_cnt)
            request(sockfd, lars++, 1);
    }
}

// wyślij ponowne zapytania o fragmenty, których nie dostaliśmy
void retry_requests(int sockfd, int force)
{
    for (int i = lar, off = get_offset(lar); i < lars; ++i)
    {
        struct segment* seg = segments + off++;
        if (force || should_retry(seg))
            request(sockfd, i, 1);
        if (off == MAX_SWS)
            off = 0;
    }
}

void download_file(int sockfd)
{
    total_packet_cnt = (total_download + PACKET_SIZE - 1) / PACKET_SIZE;
    sws = total_packet_cnt < MAX_SWS ? total_packet_cnt : MAX_SWS;
    lar = 0; lars = sws;

    // wyślij początkowe żądania
    retry_requests(sockfd, 1);
    
    while (lar < total_packet_cnt)
    {
        // poczekaj na przychodzące pakiety i je odbierz
        receive_packets(sockfd);
        // przesuń ramkę po przyjściu pakietów
        advance_frame(sockfd);
        // ponów żądania, jeśli minęło wystarczająco dużo czasu
        retry_requests(sockfd, 0);
     }
}

int main(int argc, char* argv[])
{
    parse_args(argc, argv);
    int sockfd = setup_server();
    download_file(sockfd);

    return EXIT_SUCCESS;
}
