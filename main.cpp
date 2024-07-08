#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using std::string;

enum Request { GET, POST, PATCH };

string handle_request(const Request r) {
  switch (r) {
  case GET:
    return "GET";
  case POST:
    return "POST";
  case PATCH:
    return "PATCH";
  default:
    return "BAD REQUEST";
  }
}

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "Failed to create socket\n" << std::endl;
    return 1;
  }

  sockaddr_in server_addr;
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(8080);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    std::cerr << "Failed to bind socket\n" << std::endl;
    return 1;
  }

  if (listen(server_fd, 10) == -1) {
    std::cerr << "Failed to listen on socket\n" << std::endl;
    close(server_fd);
    return 1;
  }

  std::cout << "Server is listening on port 8080\n" << std::endl;

  while (true) {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
      std::cerr << "Failed to accept connection\n" << std::endl;
      continue;
    }

    char buffer[1024];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      std::cout << "Received request:\n" << buffer << std::endl;

      const char *response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, world!";
      write(client_fd, response, std::strlen(response));
    }

    close(client_fd);
  }

  close(server_fd);
  return 0;
}
