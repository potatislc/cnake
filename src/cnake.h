#ifndef CNAKE_H
#define CNAKE_H

#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>

class Object {
public:
  enum class Type : uint8_t { NONE, NUMBER, count };
  static constexpr std::array<const char *, static_cast<size_t>(Type::count)>
      type_names{"None", "Number"};

  Object() = default;
  virtual ~Object() = default;
  Object(const Object &other) {
    std::memcpy(&m_data, &other.m_data, sizeof(m_data));
  }
  Object &operator=(const Object &other) {
    std::memcpy(&m_data, &other.m_data, sizeof(m_data));
    return *this;
  }
  Object(const Object &&) = delete;
  Object &operator=(const Object &&) = delete;

  virtual Type type() const = 0;
  virtual const char *type_name() const = 0;
  [[nodiscard]] uintptr_t *get_raw() { return &m_data; }
  [[nodiscard]] const uintptr_t *get_raw() const { return &m_data; }
  virtual std::string to_string() const = 0;
  template <typename T> static T &get(Object &object) {
    return *reinterpret_cast<T *>(object.get_raw());
  }
  template <typename T> static const T &get(const Object &object) {
    return *reinterpret_cast<const T *>(object.get_raw());
  }

  virtual Object &operator+(const Object &) const = 0;
  virtual Object &operator+=(const Object &) = 0;
  virtual Object &operator-(const Object &) const = 0;
  virtual Object &operator-=(const Object &) = 0;
  virtual Object &operator*(const Object &) const = 0;
  virtual Object &operator*=(const Object &) = 0;
  virtual Object &operator/(const Object &) const = 0;
  virtual Object &operator/=(const Object &) = 0;

private:
  uintptr_t m_data{};
};

class Number : public Object {
public:
  using number_t = double;
  using int_number_t = long long;
  template <typename T>
  using is_valid_primitive_t =
      std::bool_constant<std::is_same_v<T, int> || std::is_same_v<T, short> ||
                         std::is_same_v<T, long> || std::is_same_v<T, float> ||
                         std::is_same_v<T, double> || std::is_same_v<T, bool>>;

  Number() : Object() {}
  Number(const Object &other) : Object(other) {}
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Number(T value) {
    get<number_t>(*this) = static_cast<number_t>(value);
  }

  Type type() const override { return Type::NUMBER; }
  const char *type_name() const override {
    return type_names.at(static_cast<size_t>(type()));
  }
  std::string to_string() const override {
    return std::to_string(get<number_t>(*this));
  }

  //
  // Arithmetic operations
  //

  template <typename Op> Object &binary_op(const Object &other, Op op) const {
    return operation_result = op(get<number_t>(*this), get<number_t>(other));
  }

  template <typename Op> Object &compound_op(const Object &other, Op op) {
    auto &lhs = get<number_t>(*this);
    lhs = op(lhs, get<number_t>(other));
    return *this;
  }

  template <typename T, typename Op,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &binary_op(T value, Op op) const {
    return operation_result =
               op(get<number_t>(*this), static_cast<number_t>(value));
  }

  template <typename T, typename Op,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &compound_op(T value, Op op) {
    auto &lhs = get<number_t>(*this);
    lhs = op(lhs, static_cast<number_t>(value));
    return *this;
  }

  // '+' Operator
  Object &operator+(const Object &other) const override {
    return binary_op(other, std::plus<number_t>());
  }
  Object &operator+=(const Object &other) override {
    return compound_op(other, std::plus<number_t>());
  }
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &operator+(T value) const {
    return binary_op(value, std::plus<number_t>());
  }
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &operator+=(T value) {
    return compound_op(value, std::plus<number_t>());
  }

  // '-' Operator
  Object &operator-(const Object &other) const override {
    return binary_op(other, std::minus<number_t>());
  }
  Object &operator-=(const Object &other) override {
    return compound_op(other, std::minus<number_t>());
  }
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &operator-(T value) const {
    return binary_op(value, std::minus<number_t>());
  }
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &operator-=(T value) {
    return compound_op(value, std::minus<number_t>());
  }

  // '*' Operator
  Object &operator*(const Object &other) const override {
    return binary_op(other, std::multiplies<number_t>());
  }
  Object &operator*=(const Object &other) override {
    return compound_op(other, std::multiplies<number_t>());
  }
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &operator*(T value) const {
    return binary_op(value, std::multiplies<number_t>());
  }
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &operator*=(T value) {
    return compound_op(value, std::multiplies<number_t>());
  }

  // '/' Operator
  Object &operator/(const Object &other) const override {
    return binary_op(other, std::divides<number_t>());
  }
  Object &operator/=(const Object &other) override {
    return compound_op(other, std::divides<number_t>());
  }
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &operator/(T value) const {
    return binary_op(value, std::divides<number_t>());
  }
  template <typename T,
            typename = std::enable_if_t<is_valid_primitive_t<T>::value>>
  Object &operator/=(T value) {
    return compound_op(value, std::divides<number_t>());
  }

private:
  static thread_local Number operation_result;
};

#endif