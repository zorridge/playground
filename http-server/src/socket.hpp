#pragma once

constexpr int PORT = 4221;

struct SocketRAII
{
  int fd;
  explicit SocketRAII(int fd);
  ~SocketRAII();
  operator int() const;

  // Disable copy
  SocketRAII(const SocketRAII &) = delete;
  SocketRAII &operator=(const SocketRAII &) = delete;

  // Enable move
  SocketRAII(SocketRAII &&other) noexcept;
  SocketRAII &operator=(SocketRAII &&other) noexcept;
};

int set_up();
