#include <cstddef>
#include <cstdint>
#include <string_view>

#include "details.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const std::string_view input(reinterpret_cast<const char *>(data), size);
    const auto words = details::tokenize_line(input);
    (void)words;
    return 0;
}