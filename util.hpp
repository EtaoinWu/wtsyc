#pragma once
#include <variant>

namespace SysY {
  template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

  template <class... Args> struct variant_cast_proxy {
    std::variant<Args...> v;

    template <class... ToArgs> operator std::variant<ToArgs...>() const {
      return std::visit(
          [](auto &&arg) -> std::variant<ToArgs...> { return arg; }, v);
    }
  };

  template <class... Args>
  auto variant_cast(const std::variant<Args...> &v)
      -> variant_cast_proxy<Args...> {
    return {v};
  }

  template <typename E>
  constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
  }

  template<typename T>
  struct PointerRange {
    T *_begin, *_end;
    T *begin() const {
      return _begin;
    }
    T *end() const {
      return _end;
    }
  };
  template<typename T>
  PointerRange(T*, T*) -> PointerRange<T>;
  template<typename T>
  PointerRange(std::initializer_list<T*>) -> PointerRange<T>;
} // namespace SysY