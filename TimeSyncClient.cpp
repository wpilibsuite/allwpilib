#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <sys/time.h>

#define SERVER_IP "127.0.0.1"  // Replace with the server's IP address if different
#define PORT 5810

uint64_t getCurrentTimeInMicroseconds() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

using std::cout, std::endl;

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

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

    // Set up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (true) {
        // Get the current system time as a uint64_t
        uint64_t current_time = getCurrentTimeInMicroseconds();

        // Send the current time to the server
        sendto(sockfd, &current_time, sizeof(current_time), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // std::cout << "Sent current time (microseconds since epoch): " << current_time << std::endl;
        uint64_t server_time {0};
        int len = recvmsg(sockfd, &server_time, sizeof(server_time), 0);
        if (len != sizeof(server_time)) {
            cout << "Got only x bytes on recv: " << len << endl;
        }
        cout << "Local time: " << current_time << " server time " << server_time << endl;

        // Sleep for 1 second before sending the next time
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Close the socket
    close(sockfd);
    return 0;
}
