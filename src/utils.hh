#ifndef FCHK_UTILS_HH
#define FCHK_UTILS_HH

#include <filesystem>
#include <fmt/format.h>
#include <ranges>
#include <algorithm>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using usz = size_t;
using uptr = uintptr_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using isz = ptrdiff_t;
using iptr = intptr_t;

#define STR_(X) #X
#define STR(X)  STR_(X)

#define CAT_(X, Y) X##Y
#define CAT(X, Y)  CAT_(X, Y)

/// \brief Defer execution of a lambda until the end of the scope.
///
/// Example:
/// \code{.cpp}
///     auto file = std::fopen(...);
///     defer { if (file) std::fclose(file); };
/// \endcode
#define defer auto CAT(_defer_, __COUNTER__) = ::detail::DeferStage1{}->*[&]

namespace fs = std::filesystem;
namespace rgs = std::ranges;
namespace vws = std::ranges::views;

template <typename... arguments>
[[noreturn]] void die(fmt::format_string<arguments...> fmt, arguments&&... args) {
    fmt::print(stderr, fmt, std::forward<arguments>(args)...);
    fmt::print(stderr, "\n");
    std::exit(1);
}

namespace detail {
template <typename Callable>
struct DeferStage2 {
    Callable cb;
    ~DeferStage2() { cb(); }

    explicit DeferStage2(Callable&& _cb)
        : cb(std::forward<Callable>(_cb)) {}
};

struct DeferStage1 {
    template <typename Callable>
    DeferStage2<Callable> operator->*(Callable&& cb) {
        return DeferStage2<Callable>{std::forward<Callable>(cb)};
    }
};
} // namespace detail

namespace utils {
void ReplaceAll(
    std::string& str,
    std::string_view from,
    std::string_view to
);

auto NumberWidth(usz number, usz base = 10) -> usz;

template <bool include_newline = true>
constexpr bool IsWhitespace(char c) {
    if constexpr (include_newline) {
        if (c == '\n' or c == '\r') return true;
    }

    return c == ' ' or c == '\t' or c == '\v' or c == '\f';
}

} // namespace utils

/// Not in utils because we need it constantly.
constexpr auto Trim(std::string_view str) -> std::string_view {
    while (not str.empty() and utils::IsWhitespace(str.front())) str.remove_prefix(1);
    while (not str.empty() and utils::IsWhitespace(str.back())) str.remove_suffix(1);
    return str;
}

template <typename T>
requires std::is_enum_v<T>
auto operator+(T t) -> std::underlying_type_t<T> {
    return static_cast<std::underlying_type_t<T>>(t);
}

#endif // FCHK_UTILS_HH
