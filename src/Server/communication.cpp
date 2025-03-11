#include "communication.hpp"

/**
 * @brief Will do a loop until all the data has been sent
 * 
 * @param sock 
 * @param buffer 
 * @param length 
 * @return true on success
 * @return false on error
 */
bool sendAll(int sock, const void* buffer, size_t length) {
    const char* data = static_cast<const char*>(buffer);
    size_t totalSent = 0;

    while (totalSent < length) {
        ssize_t sent = send(sock, data + totalSent, length - totalSent, 0);
        if (sent == -1) {
            perror("Erreur lors de l'envoi");
            return false;
        }
        totalSent += sent;
    }
    std::cout<<"sent All : "<<std::endl;
    return true;
}




/**
 * @brief Receive all the data with a loop
 * 
 * @param sock 
 * @param buffer 
 * @param length 
 * @return true on success
 * @return false on error
 */
bool recvAll(int sock, void* buffer, size_t length) {
    char* data = static_cast<char*>(buffer);
    size_t totalReceived = 0;

    while (totalReceived < length) {
        ssize_t received = recv(sock, data + totalReceived, length - totalReceived, 0);
        if (received <= 0) {
            return false;
        }
        totalReceived += received;
    }
    return true;
}