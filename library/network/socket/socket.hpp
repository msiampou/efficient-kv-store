#ifndef _LIBRARY_NETWORK_SOCKET_SOCKET_HPP_
#define _LIBRARY_NETWORK_SOCKET_SOCKET_HPP_

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cassert>
#include <vector>
#include <unistd.h> 
#include <cstring>

namespace io {

class socket {
  public:
    socket() = default;

    template <typename Port>
    void create(Port& port) {
      socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
      assert(socket_fd >= 0 && "Creation of socket failed");

      int opt = 1;
      std::int32_t err = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt));
      assert(err >= 0);

      bzero(reinterpret_cast<char*>(&serv_addr), sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;   
      serv_addr.sin_addr.s_addr = INADDR_ANY;
      serv_addr.sin_port = htons(port);
    }

    template <typename Port, typename IP>
    void create(Port& port, IP& addr) {
      socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
      assert(socket_fd >= 0 && "Creation of socket failed");

      struct hostent* host = gethostbyname(addr.c_str());
      assert(host && "gethostbyname failed");

      bzero(reinterpret_cast<char*>(&serv_addr), sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy(reinterpret_cast<char*>(host->h_addr), reinterpret_cast<char*>(&serv_addr.sin_addr.s_addr), host->h_length);
      serv_addr.sin_port = htons(port);
    }

    void bind() {
      auto err = ::bind(socket_fd, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr));
      assert(err >= 0 && "Bind failed");
    }

    void listen() {
      auto err = ::listen(socket_fd, 5);
      assert(err >= 0);
    }

    std::int32_t accept() {
      socklen_t clilen = sizeof(cli_addr);
      return ::accept(socket_fd, reinterpret_cast<struct sockaddr*>(&cli_addr), &clilen);
    }

    std::int32_t connect() {
      socklen_t addrlen = sizeof(serv_addr);
      return ::connect(socket_fd, reinterpret_cast<struct sockaddr*>(&serv_addr), addrlen);
    }

    std::int32_t read() {
      char buffer[256];
      bzero(buffer, 256);
      auto msg_bytes = ::read(socket_fd, buffer, 255);
      std:: cout << buffer << std::endl;
      return msg_bytes;
    }

    std::int32_t read(std::int32_t connection_fd) {
      char buffer[256];
      bzero(buffer, 256);
      auto msg_bytes = ::read(connection_fd, buffer, 255);
      std:: cout << buffer << std::endl;
      return msg_bytes;
    }

    std::int32_t write(std::int32_t connection_fd, std::string message) {
      return ::write(connection_fd, message.c_str(), strlen(message.c_str()));
    }

    std::int32_t write(std::string message) {
      return ::write(socket_fd, message.c_str(), strlen(message.c_str()));
    }

    ~socket() {
      close(socket_fd);
    }

  private:
    std::int32_t socket_fd;
    struct sockaddr_in serv_addr, cli_addr;
};

} // namespace io

#endif // _LIBRARY_NETWORK_SOCKET_SOCKET_HPP_