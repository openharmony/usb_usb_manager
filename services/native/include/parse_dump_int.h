#ifndef USB_PARSE_DUMP_INT_H
#define USB_PARSE_DUMP_INT_H

#include <charconv>
#include <cstdint>
#include <string>
#include <system_error>

namespace OHOS {
namespace USB {
inline bool ParseDumpInt32(const std::string &text, int32_t &out)
{
    if (text.empty()) {
        return false;
    }
    const char *first = text.data();
    const char *last = first + text.size();
    auto [ptr, ec] = std::from_chars(first, last, out);
    return ec == std::errc{} && ptr == last;
}
} // namespace USB
} // namespace OHOS
#endif
