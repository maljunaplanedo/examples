#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define DOMAIN_BUFFER_SIZE 512
#define DNS_RESPONSE_BUFFER_SIZE 4096

typedef struct {
    uint16_t transaction_id;
    uint16_t flags;
    uint16_t questions;
    uint16_t answer_rrs;
    uint16_t authority_rrs;
    uint16_t additional_rrs;
} __attribute__((__packed__)) dns_query1_t;

typedef struct {
    uint16_t type;
    uint16_t class;
    uint8_t ar_name;
    uint16_t ar_type;
    uint16_t udp_payload_size;
    uint8_t hbercode;
    uint8_t ends0_v;
    uint16_t z;
    uint16_t data_length;
    uint16_t opcode;
    uint16_t oplen;
    uint64_t cookie;
} __attribute__((__packed__)) dns_query2_t;

typedef struct {
    char filler1[2];
    uint16_t type;
    char filler2[6];
    uint16_t data_length;
} __attribute__((__packed__)) dns_response_answer_t;

uint64_t random_2b()
{
    return (uint16_t)rand();
}

void encode_domain(const char* domain, char* result, size_t domain_len)
{
    result[domain_len + 1] = '\0';
    uint8_t part_len = 0;

    for (int i = (int)domain_len - 1; i >= 0; --i) {
        if (domain[i] == '.') {
            result[i + 1] = (char)part_len;
            part_len = 0;
        } else {
            result[i + 1] = domain[i];
            ++part_len;
        }
    }
    result[0] = (char)part_len;
}

void make_dns_query(
    int sock,
    const char* dns_server_ip,
    const char* domain,
    struct in_addr* result)
{
    size_t domain_len = strlen(domain);

    size_t query_len =
        sizeof(dns_query1_t) + domain_len + 2 + sizeof(dns_query2_t);
    char query_buffer[query_len];

    char* encoded_domain = query_buffer + sizeof(dns_query1_t);
    encode_domain(domain, encoded_domain, domain_len);
    domain_len += 2;

    dns_query1_t* query_part1 = (dns_query1_t*)query_buffer;
    dns_query2_t* query_part2 = (dns_query2_t*)(encoded_domain + domain_len);

    uint16_t transaction_id = random_2b();
    uint64_t cookie = random_2b() + (random_2b() << 16) + (random_2b() << 32) +
                      (random_2b() << 48);

    query_part1->transaction_id = transaction_id;
    query_part1->flags = htons(0x0120);
    query_part1->questions = htons(1);
    query_part1->answer_rrs = 0;
    query_part1->authority_rrs = 0;
    query_part1->additional_rrs = htons(1);

    query_part2->type = htons(1);
    query_part2->class = htons(1);
    query_part2->ar_name = 0;
    query_part2->ar_type = htons(0x0029);
    query_part2->udp_payload_size = htons(0x1000);
    query_part2->hbercode = 0;
    query_part2->ends0_v = 0;
    query_part2->z = 0;
    query_part2->data_length = htons(0x000c);
    query_part2->opcode = htons(0x000a);
    query_part2->oplen = htons(0x0008);
    query_part2->cookie = cookie;

    struct sockaddr_in addr = {
        .sin_port = htons(53),
        .sin_family = AF_INET,
    };
    inet_aton(dns_server_ip, &addr.sin_addr);

    sendto(
        sock, query_buffer, query_len, 0, (struct sockaddr*)&addr, sizeof addr);

    char buffer[DNS_RESPONSE_BUFFER_SIZE];

    recvfrom(sock, buffer, DNS_RESPONSE_BUFFER_SIZE, 0, NULL, NULL);

    uint16_t reply_transaction_id = *((uint16_t*)buffer);
    assert(transaction_id == reply_transaction_id);

    dns_response_answer_t* answer =
        (dns_response_answer_t*)(buffer + domain_len + 16);

    while (ntohs(answer->type) != 0x0001) { // Type A
        answer = (dns_response_answer_t*)((char*)answer + sizeof(dns_response_answer_t)
                                          + ntohs(answer->data_length));
    }

    uint32_t* data = (uint32_t*)((char*)answer + sizeof(dns_response_answer_t));
    result->s_addr = *data;
}

int main()
{
    srand(time(NULL));

    const char* dns_server_ip = "8.8.8.8";
    char domain[DOMAIN_BUFFER_SIZE];
    struct in_addr ip;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    while (scanf("%s", domain) != EOF) {
        make_dns_query(sock, dns_server_ip, domain, &ip);
        printf("%s\n", inet_ntoa(ip));
    }

    close(sock);
    return 0;
}

