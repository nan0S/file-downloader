#include <time.h>
#include "init.h"

// maksymalna wielkość ramki
#define MAX_SWS 1500
// czas, po którym wysyłamy pakiet po raz kolejny
const double RETRY_TIME = 0.04;
// liczba pakietów, które przyszły w obecnej ramce
int recv_frame = 0;
// liczba bajtów, które pobraliśmy w sumie
int recv_len = 0;

struct segment
{
    // czy segment został pobrany
    int received;
    // ostatni czas kiedy, go wysłaliśmy
    struct timespec last_time;
    // dane
    char data[PACKET_SIZE];
} segments[MAX_SWS];

// zresetuj segment
void szero(struct segment* seg)
{
    seg->received = 0;
    clock_gettime(CLOCK_REALTIME, &seg->last_time);
}

// zwraca indeks pakietu w tablicy segmentów
int get_offset(int packet_idx)
{
    return packet_idx % MAX_SWS;
}

// zwraca informacje czy segment powinien być ponownie wysłany
// (nie dostaliśmy go do tej pory i minęło wystarczająco czasu od
//  od poprzedniej żądania)
int should_retry(struct segment* seg)
{
    if (seg->received)
        return 0;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    if (ts.tv_nsec < seg->last_time.tv_nsec)
    {
        ts.tv_nsec += 1000000000;
        --ts.tv_sec;
    }
    double passed = (ts.tv_sec - seg->last_time.tv_sec) + (ts.tv_nsec - seg->last_time.tv_nsec) * 0.000000001;
    return passed > RETRY_TIME;
}

// dodaj nowy segment - po przyjściu od serwera
// zwraca informację, czy jest to nowy fragment pliku
int add_new_segment(char* data, int lar)
{
    int start, len;
    sscanf(data + 5, "%d %d", &start, &len);
    while (*data != '\n')
        ++data;
    ++data;
    
    if (start < lar * PACKET_SIZE)
        return 0;

    int offset = get_offset(start / PACKET_SIZE);
    struct segment* seg = segments + offset;
    if (seg->received)
        return 0;

    seg->received = 1;
    ++recv_frame;
    recv_len += len;
    memcpy(seg->data, data, len);

    return 1;
}