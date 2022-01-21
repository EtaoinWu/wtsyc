#pragma once
#include <iterator>
#include <sstream>
#include <variant>

namespace SysY {
  template <typename T>
  std::vector<T> &merge_into(std::vector<T> &a, const std::vector<T> &b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
  }

  template <typename T>
  std::vector<T> &merge_into(std::vector<T> &a, const T &b) {
    a.push_back(b);
    return a;
  }

  template <typename T, typename Func>
  std::vector<T> operator>>(const std::vector<T> &l, Func f) {
    std::vector<T> result;
    for (const auto &item : l) {
      merge_into(result, f(item));
    }
    return result;
  }

  template <typename T>
  std::vector<T> monad_join(const std::vector<std::vector<T>> v) {
    std::vector<T> result;
    for (const auto &item : v) {
      merge_into(result, item);
    }
    return result;
  }

  struct do_in_order {
    template <typename T> do_in_order(std::initializer_list<T> &&) {}
  };

  namespace details {
    template <typename V> void concat_helper(V &l, const V &r) {
      l.insert(l.end(), r.begin(), r.end());
    }
    template <class V> void concat_helper(V &l, V &&r) {
      l.insert(
        l.end(), std::make_move_iterator(r.begin()),
        std::make_move_iterator(r.end()));
    }
  } // namespace details

  template <typename T, typename... A>
  std::vector<T> concat(std::vector<T> v1, A &&... vr) {
    std::size_t s = v1.size();
    do_in_order{s += vr.size()...};
    v1.reserve(s);
    do_in_order{(details::concat_helper(v1, std::forward<A>(vr)), 0)...};
    return std::move(v1); // rvo blocked
  }

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

  template <typename T> struct PointerRange {
    T *_begin, *_end;
    T *begin() const { return _begin; }
    T *end() const { return _end; }
  };
  template <typename T> PointerRange(T *, T *) -> PointerRange<T>;
  template <typename T>
  PointerRange(std::initializer_list<T *>) -> PointerRange<T>;

  template <typename U, typename T, typename Func>
  inline std::vector<U> map_to_vector(T &&v, Func &&f) {
    typename std::vector<U> ans;
    ans.reserve(end(v) - begin(v));
    for (const auto &x : v) {
      ans.push_back(f(x));
    }
    return ans;
  }

  inline std::string
  join(const std::vector<std::string> &strings, const char *delim) {
    std::ostringstream oss;
    std::copy(
      strings.begin(), strings.end(),
      std::ostream_iterator<std::string>(oss, delim));
    return oss.str();
  }
} // namespace SysY