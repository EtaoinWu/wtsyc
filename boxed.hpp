#pragma once
#include <cstring>
#include <gc/gc_cpp.h>
#include <string_view>

namespace SysY {
  template <typename T> struct gc_ptr {
    T *_data;
    operator T *() { return _data; }
    gc_ptr(T *ptr_) : _data(ptr_) {}
    gc_ptr() = default;
    gc_ptr(const gc_ptr &) = default;
    gc_ptr(gc_ptr &&) = default;
    ~gc_ptr() = default;
    gc_ptr &operator=(const gc_ptr &) = default;
    T *raw() { return _data; }
  };

  template <typename T, typename... Args> gc_ptr<T> make_gc(Args &&... args) {
    void *mem = GC_MALLOC(sizeof(T));
    T *ptr = new (mem) T(std::forward<Args>(args)...);
    return ptr;
  }

  using gc_str = gc_ptr<char>;

  gc_str gc_copy_str(const char *str, size_t len = 0);
  inline gc_str gc_copy_str(std::string_view v) {
    return gc_copy_str(v.data(), v.length());
  }
} // namespace SysY
