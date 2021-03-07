#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>

template <typename T> class my_vector {
public:
  using iterator = T *;
  using const_iterator = const T *;
  using reference = T &;
  using const_reference = const T &;

  my_vector() = default;

  explicit my_vector(size_t size) noexcept
      : size_{size}, capacity_{size}, array{new T[size]} {}

  template <typename T1> my_vector(T1 *begin_pos, T1 *end_pos) {
    assert(begin_pos != nullptr && end_pos != nullptr);
    assert(begin_pos != end_pos);

    size_t diff_iterator = end_pos - begin_pos;
    reserve(diff_iterator);
    for (auto iter = begin(); begin_pos != end_pos; ++iter, ++begin_pos) {
      *iter = *begin_pos;
      ++size_;
    }
  }

  template <typename T1>
  my_vector(std::initializer_list<T1> lst)
      : size_{lst.size()}, capacity_{lst.size()}, array(new T1[lst.size()]) {

    std::copy(lst.begin(), lst.end(), array);
  }

  my_vector(const my_vector &rhs)
      : size_{rhs.size_}, capacity_{rhs.capacity_}, array{new T[rhs.size_]} {

    for (size_t i = 0; i < size_; ++i) {
      array[i] = rhs.array[i];
    }
  }

  my_vector(my_vector &&rhs) noexcept
      : size_{std::move(rhs.size_)}, capacity_{std::move(rhs.capacity_)},
        array{std::move(rhs.array)} {

    rhs.size_ = 0;
    rhs.capacity_ = 0;
    rhs.array = nullptr;
  }

  ~my_vector() noexcept {
    if (array != nullptr) {
      delete[] array;
    }
  }

  my_vector &operator=(const my_vector &rhs) {
    if (this == &rhs) {
      return *this;
    }
    my_vector tmp{rhs};
    std::swap(size_, tmp.size_);
    std::swap(capacity_, tmp.capacity_);
    std::swap(array, tmp.array);

    return *this;
  }

  my_vector &operator=(my_vector &&rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }
    size_ = std::move(rhs.size_);
    capacity_ = std::move(rhs.size_);
    array = std::move(rhs.array);
    rhs.size_ = 0;
    rhs.capacity_ = 0;
    rhs.array = nullptr;

    return *this;
  }

  iterator begin() { return array; }
  iterator end() { return array + size_; }
  const_iterator begin() const { return array; }
  const_iterator end() const { return array + size_; }

  reference operator[](size_t index) { return array[index]; }
  const_reference operator[](size_t index) const { return array[index]; }

  size_t size() const { return size_; }
  size_t capacity() const { return capacity_; }
  bool empty() const { return size_ == 0; }

  void reserve(size_t new_alloc) {
    if (new_alloc <= capacity_) {
      return;
    }
    T *alloc = new T[new_alloc];
    if (size_ != 0) {
      for (size_t i = 0; i < size_; ++i) {
        alloc[i] = array[i];
      }
      delete[] array;
    }
    array = alloc;
    capacity_ = new_alloc;
  }

  void check_capacity() {
    if (capacity_ == 0) {
      reserve(1);
    } else if (size_ == capacity_) {
      reserve(2 * capacity_);
    }
  }

  template <typename... U> void emplace_back(U &&...data) {
    check_capacity();
    new (array + size_) T{std::forward<U>(data)...};
    ++size_;
  }

  void push_back(const T &value) { emplace_back(value); }

  void push_back(T &&value) { emplace_back(std::move(value)); }

  void push_front(const T &value) {
    check_capacity();
    for (size_t i = size(); i > 0; --i) {
      array[i] = array[i - 1];
    }
    array[0] = value;
    ++size_;
  }

  void push_front(T &&value) {
    check_capacity();
    for (size_t i = size(); i > 0; --i) {
      array[i] = array[i - 1];
    }
    array[0] = std::move(value);
    ++size_;
  }

  void pop_back() {
    assert(!empty());
    --size_;
  }

  iterator insert(iterator pos, const_reference value) {
    assert(pos != nullptr);
    assert(pos >= begin() && pos <= end());

    check_capacity();
    for (auto it = end() - 1; pos != it; --it) {
      *it = *(it - 1);
    }
    *pos = value;
    ++size_;

    return pos;
  }

  iterator erase(iterator pos) {
    assert(pos != nullptr);
    assert(pos >= begin() && pos < end());

    if (pos == end()) {
      return pos;
    }
    for (auto it = pos + 1; it != end(); ++it) {
      *(it - 1) = *it;
    }
    --size_;

    return pos;
  }

  iterator erase(iterator begin_pos, iterator end_pos) {
    assert(begin_pos != nullptr && end_pos != nullptr);
    assert(begin_pos >= begin() && begin_pos <= end());
    assert(begin_pos < end_pos);

    size_t diff_iter = end_pos - begin_pos;
    for (auto it = begin_pos; it != end(); ++it) {
      *it = *(it + diff_iter);
    }
    size_ -= diff_iter;

    return begin_pos;
  }

  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }

  void resize(size_t new_size) {
    reserve(new_size);
    size_ = new_size;
  }

private:
  size_t size_{};
  size_t capacity_{};
  T *array{};
};
