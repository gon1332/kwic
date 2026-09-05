#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace details
{
auto read_file(const std::string &p_path) -> std::string;
auto tokenize_line(std::string_view p_line) -> std::vector<std::string_view>;
} // namespace details
