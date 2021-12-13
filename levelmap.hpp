#pragma once

#include <memory>
#include <unordered_map>

namespace SysY::Utility {
  template <typename Key, typename Value>
  using underlying_map = std::unordered_map<Key, Value>;

  template <typename Key, typename Value>
  class levelmap : public std::enable_shared_from_this<levelmap<Key, Value>> {
  public:
    using key_type = Key;
    using value_type = Value;
    using ptr_t = std::shared_ptr<levelmap>;
    using c_ptr_t = std::shared_ptr<const levelmap>;

  private:
    const c_ptr_t parent;
    mutable underlying_map<key_type, value_type> data;

  public:
    levelmap() : parent{nullptr}, data{} {}
    levelmap(const c_ptr_t &parent_) : parent{parent_}, data{} {}
    ptr_t clone() const {
      return std::make_shared<levelmap>(this->shared_from_this());
    }

    auto count(const key_type &k) const -> decltype(data.count(k)) {
      auto cur_count = data.count(k);
      if (cur_count > 0) {
        return cur_count;
      }
      if (parent) {
        return parent->count(k);
      }
      return 0;
    }

    auto insert(const key_type &k, const value_type &v)
        -> decltype(data.insert({k, v})) {
      return data.insert({k, v});
    }

    auto at(const key_type &k) const -> decltype(data.at(k)) const {
      if (data.count(k)) {
        return data.at(k);
      }
      if (parent) {
        return (*parent).at(k);
      }
      return data.at(k);
    }

    auto at(const key_type &k) -> decltype(data.at(k)) {
      if (data.count(k)) {
        return data.at(k);
      }
      if (parent) {
        return (*parent).at(k);
      }
      return data.at(k);
    }
  };
} // namespace SysY::Utility
