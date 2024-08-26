#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <ctime>

#define PORT 5810
#define BUFFER_SIZE 8

uint64_t getCurrentTimeInMicroseconds() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Enable the SO_TIMESTAMP option
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_TIMESTAMP, &enable, sizeof(enable)) < 0) {
        perror("Setting SO_TIMESTAMP failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Set up the server address and bind it to the socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (true) {
        // Prepare structures for ancillary data
        struct iovec iov[1];
        struct msghdr msg;
        char control_buf[CMSG_SPACE(sizeof(struct timeval))];
        struct timeval *recv_time;

        // Clear buffer and receive message
        memset(buffer, 0, BUFFER_SIZE);
        memset(&msg, 0, sizeof(msg));
        memset(control_buf, 0, sizeof(control_buf));

        // Set up the message header
        iov[0].iov_base = buffer;
        iov[0].iov_len = BUFFER_SIZE;
        msg.msg_name = &client_addr;
        msg.msg_namelen = sizeof(client_addr);
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_control = control_buf;
        msg.msg_controllen = sizeof(control_buf);

        int n = recvmsg(sockfd, &msg, 0);
        if (n < 0) {
            perror("Failed to receive message");
            continue;
        }

        // Retrieve the timestamp
        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMP) {
            recv_time = (struct timeval *)CMSG_DATA(cmsg);
            std::cout << "Packet received at (seconds): " << recv_time->tv_sec 
                      << " (microseconds): " << recv_time->tv_usec << std::endl;
        }

        // Get the current time as a 64-bit integer
        uint64_t current_time = getCurrentTimeInMicroseconds();

        // Send the current time back to the client as a uint64_t
        sendto(sockfd, &current_time, sizeof(current_time), 0, (struct sockaddr *)&client_addr, client_len);

        std::cout << "Sent current time (microseconds since epoch) to client: " << current_time << std::endl;
    }

    // Close the socket
    close(sockfd);
    return 0;
}
