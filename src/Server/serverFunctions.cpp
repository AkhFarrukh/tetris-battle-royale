#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

int main(){
  int server_id = socket(AF_INET, SOCK_STREAM, 0);
  if(server_id <= 0){
    std::cerr << "socket creation error \n";
  }

  int opt = 1;
  if(setsockopt(server_id, SOL_SOCKET, SO_REUSEADDR));
  return 0;
}
