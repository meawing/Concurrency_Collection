#pragma once

#include <iostream>

namespace lines {

class Logger {
public:
    template <class T>
    Logger& operator<<(const T& obj) {
        stream_ << obj;

        return *this;
    }

    Logger& operator<<(std::basic_ostream<char>& (*func)(std::basic_ostream<char>&)) {
        stream_ << func;

        return *this;
    }

private:
    std::ostream& stream_ = std::cerr;
};

Logger& DefaultLogger();

}  // namespace lines
