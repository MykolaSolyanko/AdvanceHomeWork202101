#pragma once

#include "my_vector.hpp"

template <typename T, typename Container = my_vector<T>> class my_stack {
public:
  my_stack() = default;

  void push(const T &val) { cont.emplace_back(val); }
  void push(T &&val) { cont.emplace_back(std::move(val)); }
  void pop() { cont.pop_back(); }

  template <typename... Args> void emplace(Args &&...args) {
    cont.emplace_back(std::forward<Args>(args)...);
  }

  T &top() { return cont.back(); }
  bool empty() const { return cont.empty(); }
  size_t size() const { return cont.size(); }

private:
  Container cont;
};
