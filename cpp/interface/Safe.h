#ifndef SAFE_H
#define SAFE_H

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <stdexcept>
#include <string>

// Ошибка во входных данных пользователя (код возврата 1)
struct InputError : std::runtime_error {
    explicit InputError(const std::string& m) : std::runtime_error(m) {}
};

// Ошибка работы с файлом (код возврата 2)
struct IoError : std::runtime_error {
    explicit IoError(const std::string& m) : std::runtime_error(m) {}
};

// Безопасный разбор целого числа: никаких исключений std::stoi, только понятное сообщение
inline long long parseInt(const std::string& s, const std::string& what,
                          long long lo = LLONG_MIN, long long hi = LLONG_MAX) {
    if (s.empty()) throw InputError(what + ": пустое значение");
    char* end = nullptr;
    errno = 0;
    long long v = std::strtoll(s.c_str(), &end, 10);
    if (end == s.c_str() || *end != '\0')
        throw InputError(what + ": \"" + s + "\" не является целым числом");
    if (errno == ERANGE || v < lo || v > hi)
        throw InputError(what + ": значение \"" + s + "\" вне допустимого диапазона [" +
                         std::to_string(lo) + "; " + std::to_string(hi) + "]");
    return v;
}

inline std::string upper(std::string s) {
    for (char& c : s)
        if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    return s;
}

#endif
