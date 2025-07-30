#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "http_response.hpp"

std::vector<std::string> split_string(std::string_view s, std::string_view delimiter);
HttpResponse get(const HttpRequest &client_fd, std::string_view dir);
HttpResponse post(const HttpRequest &client_fd, std::string_view dir);
