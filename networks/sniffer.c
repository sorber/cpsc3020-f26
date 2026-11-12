#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>

#define BUFFER_SIZE 65536

int main()
{
    int raw_sock;
    unsigned char buffer[BUFFER_SIZE];

    // Create a raw socket listening to ALL incoming Ethernet frames
    raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_sock < 0)
    {
        perror("soccket failed (need sudo?)");
        return EXIT_FAILURE;
    }

    printf("Sniffing data link layer frames... Send a UDP packet containing 'HELLO' to trigger!\n");

    while (true)
    {
        ssize_t frame_length = recv(raw_sock, buffer, BUFFER_SIZE, 0);
        if (frame_length < 0)
        {
            perror("Recv failed");
            close(raw_sock);
            return EXIT_FAILURE;
        }

        // Look for the string "HELLO" anywhere in the frame
        unsigned char *payload_match = memmem(buffer, frame_length, "HELLO", 5);

        if (payload_match)
        {
            printf("\n--- [Captured Data Link Layer Frame (%ld bytes)] ---\n", frame_length);

            // Peel back the layers
            struct ethhdr *eth = (struct ethhdr *)buffer;

            // Check if the Ethernet frame encapsulates an IPv4 packet (0x0800)
            if (ntohs(eth->h_proto) == ETH_P_IP)
            {

                // IP header starts after the ethernet header
                struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

                // Extract IP strings from the structure fields
                struct in_addr src_addr, dst_addr;
                src_addr.s_addr = ip->saddr;
                dst_addr.s_addr = ip->daddr;

                // Print Layer 2 (Link Layer)
                printf("1. LINK LAYER:    Src MAC: %02x:%02x:%02x:%02x:%02x:%02x "
                       "-> Dst MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                       eth->h_source[0], eth->h_source[1], eth->h_source[2],
                       eth->h_source[3], eth->h_source[4], eth->h_source[5],
                       eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
                       eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);

                // Print Layer 3 (Network Layer)
                printf("2. NETWORK LAYER: Src IP:  %s -> Dst IP:  %s\n",
                       inet_ntoa(src_addr), inet_ntoa(dst_addr));

                // If it's UDP, we can step down to Layer 4 (Transport)
                if (ip->protocol == IPPROTO_UDP)
                {
                    // UDP header starts after the variable IP header length (ihl * 4 bytes)
                    struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct ethhdr) + (ip->ihl * 4));

                    printf("3. TRANSPORT LYR: Src Port: %d -> Dst Port: %d (UDP)\n",
                           ntohs(udp->source), ntohs(udp->dest));
                }

                // Print Layer 7 (Application Layer Payload)
                printf("4. APP DATA:      Payload triggered: \"%s\"\n", payload_match);
                break; // Exit after capturing the targeted packet
            }
        }
    }

    close(raw_sock);
    return EXIT_SUCCESS;
}
