#include "http_response.hpp"
#include <sstream>

HttpResponse &HttpResponse::set_status(const std::string &status_line)
{
  status_ = status_line;
  return *this;
}

HttpResponse &HttpResponse::set_header(const std::string &key, const std::string &value)
{
  headers_[key] = value;
  return *this;
}

HttpResponse &HttpResponse::set_body(const std::string &body)
{
  body_ = body;
  return *this;
}

std::string HttpResponse::build() const
{
  std::ostringstream oss;
  oss << status_ << "\r\n";
  for (const auto &[k, v] : headers_)
  {
    oss << k << ": " << v << "\r\n";
  }
  oss << "\r\n";
  oss << body_;
  return oss.str();
}
