#pragma once

#include <string_view>
#include <vector>

namespace details
{
auto tokenize_line(std::string_view p_line) -> std::vector<std::string_view>;
} // namespace details
