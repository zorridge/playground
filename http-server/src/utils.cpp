#include <fstream>
#include <sstream>
#include <iostream>
#include "gzip/compress.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "utils.hpp"

std::vector<std::string> split_string(std::string_view s, std::string_view delimiter)
{
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = s.find(delimiter);
  while (end != std::string_view::npos)
  {
    if (end > start)
    {
      tokens.emplace_back(s.substr(start, end - start));
    }
    start = end + delimiter.length();
    end = s.find(delimiter, start);
  }

  if (start < s.size())
  {
    tokens.emplace_back(s.substr(start));
  }

  return tokens;
}

HttpResponse get(const HttpRequest &req, std::string_view dir)
{
  std::vector<std::string> pathSegments = split_string(req.target(), "/");
  HttpResponse response;

  if (req.target() == "/")
  {
    return response.set_status("HTTP/1.1 200 OK");
  }

  if (pathSegments.size() > 1 && pathSegments[0] == "files")
  {
    std::string file_path = std::string{dir} + pathSegments[1];
    std::ifstream file{file_path, std::ios::binary};
    if (!file.is_open())
    {
      return response.set_status("HTTP/1.1 404 Not Found");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string body = buffer.str();
    response.set_status("HTTP/1.1 200 OK")
        .set_header("Content-Type", "application/octet-stream")
        .set_header("Content-Length", std::to_string(body.size()))
        .set_body(body);

    return response;
  }

  if (pathSegments.size() > 1 && pathSegments[0] == "echo")
  {
    const std::string &body = pathSegments[1];
    bool gzip_requested = false;
    auto it = req.headers().find("Accept-Encoding");
    if (it != req.headers().end() && it->second.find("gzip") != std::string::npos)
    {
      gzip_requested = true;
    }

    std::string response_body = body;
    if (gzip_requested)
    {
      response_body = gzip::compress(body.data(), body.size());
      response.set_header("Content-Encoding", "gzip");
    }

    response.set_status("HTTP/1.1 200 OK")
        .set_header("Content-Type", "text/plain")
        .set_header("Content-Length", std::to_string(response_body.size()))
        .set_body(response_body);

    return response;
  }

  if (!pathSegments.empty() && pathSegments[0] == "user-agent")
  {
    std::string_view body;
    auto it = req.headers().find("User-Agent");
    if (it != req.headers().end())
    {
      body = it->second;
    }
    else
    {
      body = "";
    }

    response.set_status("HTTP/1.1 200 OK")
        .set_header("Content-Type", "text/plain")
        .set_header("Content-Length", std::to_string(body.size()))
        .set_body(std::string(body));

    return response;
  }

  return response.set_status("HTTP/1.1 404 Not Found");
}

HttpResponse post(const HttpRequest &req, std::string_view dir)
{
  std::vector<std::string> pathSegments = split_string(req.target(), "/");
  HttpResponse response;

  if (pathSegments.size() > 1 && pathSegments[0] == "files")
  {
    std::string file_path = std::string(dir) + "/" + pathSegments[1];
    std::ofstream file{file_path, std::ios::binary};
    if (!file.is_open())
    {
      return response.set_status("HTTP/1.1 500 Internal Server Error");
    }

    file.write(req.body().data(), req.body().size());

    return response.set_status("HTTP/1.1 201 Created");
  }

  return response.set_status("HTTP/1.1 404 Not Found");
}
