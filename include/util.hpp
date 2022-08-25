#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <vector>

namespace Util {

template <typename T>
void ValueToBuffer(T value, std::vector<char>& buffer, bool net = false) {
    std::vector<char> data(sizeof(value));
    std::memcpy(data.data(), &value, sizeof(value));
    if (net) {
        std::for_each(data.rbegin(), data.rend(), [&](const char c) { buffer.push_back(c); });
    } else {
        std::for_each(data.begin(), data.end(), [&](const char c) { buffer.push_back(c); });
    }
}

}  // namespace Util

#endif
