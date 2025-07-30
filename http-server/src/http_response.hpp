#pragma once
#include <string>
#include <map>

class HttpResponse
{
public:
  HttpResponse &set_status(const std::string &status_line);
  HttpResponse &set_header(const std::string &key, const std::string &value);
  HttpResponse &set_body(const std::string &body);
  std::string build() const;

private:
  std::string status_ = "HTTP/1.1 200 OK";
  std::map<std::string, std::string> headers_;
  std::string body_;
};