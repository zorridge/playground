#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

class HttpRequest
{
public:
  explicit HttpRequest(std::string_view buffer);

  bool is_valid() const { return valid; }
  std::string_view method() const { return method_; }
  std::string_view target() const { return target_; }
  std::string_view version() const { return version_; }
  const std::map<std::string, std::string> &headers() const { return headers_; }
  std::string_view body() const { return body_; }

private:
  bool valid = false;
  std::string method_;
  std::string target_;
  std::string version_;
  std::map<std::string, std::string> headers_;
  std::string body_;

  static std::string_view trim(std::string_view s);
  static std::map<std::string, std::string> parse_headers(std::string_view headers_block);
};
