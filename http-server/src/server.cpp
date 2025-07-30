#include <csignal>
#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>

#include "http_request.hpp"
#include "http_response.hpp"
#include "socket.hpp"
#include "utils.hpp"

constexpr size_t BUFFER_SIZE = 1024;

std::atomic<bool> running{true};
int g_server_fd = -1;
void handle_signal(int)
{
  running = false;
  if (g_server_fd >= 0)
    close(g_server_fd);
}

void handle_client(SocketRAII client_fd, std::string_view dir)
{
  constexpr size_t BUFFER_SIZE = 4096;
  char buffer[BUFFER_SIZE];

  while (true)
  {
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0)
      break;

    buffer[bytes_received] = '\0';

    HttpRequest req{std::string_view(buffer, bytes_received)};
    if (!req.is_valid())
      break;

    HttpResponse response;
    bool close_connection = false;

    auto it = req.headers().find("Connection");
    if (it != req.headers().end() && it->second == "close")
    {
      close_connection = true;
    }

    if (req.method() == "GET")
      response = get(req, dir);
    else if (req.method() == "POST")
      response = post(req, dir);
    else
      response.set_status("HTTP/1.1 405 Method Not Allowed");

    if (close_connection)
      response.set_header("Connection", "close");

    std::string response_str = response.build();
    send(client_fd, response_str.c_str(), response_str.size(), 0);

    if (close_connection)
      break;
  }
}

int main(int argc, char *argv[])
{
  std::signal(SIGINT, handle_signal);
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string directory = "/tmp/";
  for (int i = 1; i < argc; ++i)
  {
    std::string_view arg = argv[i];
    if (arg == "--directory" && i + 1 < argc)
    {
      directory = argv[i + 1];
      ++i;
    }
  }
  if (directory.empty())
  {
    std::cerr << "No directory specified. Use --directory <path>\n";
    return 1;
  }
  std::cout << "Serving directory: " << directory << std::endl;

  g_server_fd = set_up();
  if (g_server_fd < 0)
    return 1;
  SocketRAII server_fd{g_server_fd};

  std::cout << "Server listening...\n";
  std::vector<std::thread> threads;

  while (running)
  {
    sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
    int raw_client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_addr_len);
    if (raw_client_fd < 0)
    {
      std::cerr << "Accept failed\n";
      continue;
    }

    SocketRAII client_fd{raw_client_fd};
    threads.emplace_back([fd = std::move(client_fd), directory]() mutable
                         { handle_client(std::move(fd), directory); });
  }

  for (auto &t : threads)
  {
    if (t.joinable())
      t.join();
  }
  std::cout << "Server shutting down\n";
  return 0;
}
