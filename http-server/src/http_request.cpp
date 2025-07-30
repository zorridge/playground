#include <iostream>

#include "http_request.hpp"
#include "utils.hpp"

HttpRequest::HttpRequest(std::string_view buffer)
{
  // Parse request line
  size_t line_end = buffer.find("\r\n");
  if (line_end == std::string_view::npos)
    line_end = buffer.find('\n');
  if (line_end == std::string_view::npos)
  {
    std::cerr << "Malformed request: no line ending\n";
    return;
  }
  std::string_view request_line = buffer.substr(0, line_end);

  size_t method_end = request_line.find(' ');
  if (method_end == std::string_view::npos)
  {
    std::cerr << "Malformed request line: no method\n";
    return;
  }
  size_t target_start = method_end + 1;
  size_t target_end = request_line.find(' ', target_start);
  if (target_end == std::string_view::npos)
  {
    std::cerr << "Malformed request line: no HTTP version\n";
    return;
  }

  method_ = trim(request_line.substr(0, method_end));
  target_ = trim(request_line.substr(target_start, target_end - target_start));
  version_ = trim(request_line.substr(target_end + 1));

  // Parse headers
  size_t header_start = line_end + ((buffer[line_end] == '\r' && buffer[line_end + 1] == '\n') ? 2 : 1);
  size_t headers_end = buffer.find("\r\n\r\n", header_start);
  size_t header_terminator_length = 4;
  if (headers_end == std::string_view::npos)
  {
    headers_end = buffer.find("\n\n", header_start);
    header_terminator_length = 2;
  }
  if (headers_end == std::string_view::npos)
  {
    std::cerr << "Malformed request: no header terminator\n";
    return;
  }
  std::string_view headers_block = buffer.substr(header_start, headers_end - header_start);
  headers_ = parse_headers(headers_block);

  // Parse body (if any)
  size_t body_start = headers_end + header_terminator_length;
  if (body_start < buffer.size())
    body_ = buffer.substr(body_start);
  else
    body_ = {};

  valid = true;
}

std::string_view HttpRequest::trim(std::string_view s)
{
  auto start = s.find_first_not_of(" \t\r\n");
  auto end = s.find_last_not_of(" \t\r\n");
  if (start == std::string_view::npos)
    return {};
  return s.substr(start, end - start + 1);
}

std::map<std::string, std::string> HttpRequest::parse_headers(std::string_view headers_block)
{
  std::map<std::string, std::string> headers;
  auto lines = split_string(headers_block, "\r\n");
  if (lines.size() == 1)
  {
    lines = split_string(headers_block, "\n");
  }
  for (const auto &line : lines)
  {
    auto colon_pos = line.find(':');
    if (colon_pos == std::string::npos)
      continue;
    std::string_view key = trim(std::string_view(line).substr(0, colon_pos));
    std::string_view value = trim(std::string_view(line).substr(colon_pos + 1));
    headers[std::string(key)] = std::string(value);
  }
  return headers;
}
