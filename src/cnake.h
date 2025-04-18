#ifndef CNAKE_H
#define CNAKE_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

class Object {
public:
  enum class Type : uint8_t { NONE, NUMBER, count };
  static constexpr std::array<const char *, static_cast<size_t>(Type::count)>
      type_names{"None", "Number"};

  virtual ~Object() = default;

  virtual std::shared_ptr<Object> clone() const = 0;
  virtual const void *get_raw() const = 0;
  virtual Type type() const = 0;
  virtual const char *type_name() const = 0;
  virtual std::string to_string() const = 0;

  virtual Object &operator+(const Object &) const = 0;
  virtual Object &operator+=(const Object &) = 0;
  virtual Object &operator-(const Object &) const = 0;
  virtual Object &operator-=(const Object &) = 0;
  virtual Object &operator*(const Object &) const = 0;
  virtual Object &operator*=(const Object &) = 0;
  virtual Object &operator/(const Object &) const = 0;
  virtual Object &operator/=(const Object &) = 0;
};

class None : public Object {
public:
  static None failsafe_none;
  static const uint64_t failsafe_null;

  std::shared_ptr<Object> clone() const override {
    return std::make_shared<None>(*this);
  }
  const void *get_raw() const override { return &failsafe_null; }
  Type type() const override { return Type::NONE; }
  const char *type_name() const override {
    return type_names.at(static_cast<size_t>(Object::Type::NONE));
  }
  std::string to_string() const override { return ""; }

  Object &operator+(const Object &other) const override {
    if (other.type() == type())
      return failsafe_none;
    return other + *this;
  }
  Object &operator+=(const Object &) override { return *this; }
  Object &operator-(const Object &other) const override {
    if (other.type() == type())
      return failsafe_none;
    return other - *this;
  }
  Object &operator-=(const Object &) override { return *this; }
  Object &operator*(const Object &other) const override {
    if (other.type() == type())
      return failsafe_none;
    return other * *this;
  }
  Object &operator*=(const Object &) override { return *this; }
  Object &operator/(const Object &other) const override {
    if (other.type() == type())
      return failsafe_none;
    return other / *this;
  }
  Object &operator/=(const Object &) override { return *this; }
};

class Number : public Object {
public:
  using number_t = double;
  using int_number_t = long long;
  Number() : m_value{} {}
  Number(const Object &other) : m_value(as_number(other)) {}
  Number(number_t value) : m_value(value) {}
  ~Number() override = default;

  std::shared_ptr<Object> clone() const override {
    return std::make_shared<Number>(*this);
  }
  const void *get_raw() const override { return &m_value; }
  Type type() const override { return Type::NUMBER; }
  const char *type_name() const override {
    return type_names.at(static_cast<size_t>(type()));
  }
  std::string to_string() const override { return std::to_string(m_value); }

  Number &operator+(const Object &other) const override {
    return result = m_value + as_number(other);
  }

  Number &operator+=(const Object &other) override {
    m_value += as_number(other);
    return *this;
  }

  Number &operator+(number_t value) const { return result = m_value + value; }

  Number &operator+=(number_t value) {
    m_value += value;
    return *this;
  }

  Number &operator-(const Object &other) const override {
    return result = m_value - as_number(other);
  }

  Number &operator-=(const Object &other) override {
    m_value -= as_number(other);
    return *this;
  }

  Number &operator-(number_t value) const { return result = m_value - value; }

  Number &operator-=(number_t value) {
    m_value -= value;
    return *this;
  }

  Number &operator*(const Object &other) const override {
    return result = m_value * as_number(other);
  }

  Number &operator*=(const Object &other) override {
    m_value *= as_number(other);
    return *this;
  }

  Number &operator*(number_t value) const { return result = m_value * value; }

  Number &operator*=(number_t value) {
    m_value *= value;
    return *this;
  }

  Number &operator/(const Object &other) const override {
    return result = m_value / as_number(other);
  }

  Number &operator/=(const Object &other) override {
    m_value /= as_number(other);
    return *this;
  }

  Number &operator/(number_t value) const { return result = m_value / value; }

  Number &operator/=(number_t value) {
    m_value /= value;
    return *this;
  }

private:
  number_t m_value;

  number_t as_number(const Object &other) const {
    // Fast reinterpret no branching
    return *reinterpret_cast<const number_t *>(other.get_raw()) *
           (type() == other.type());
    // return reinterpret_cast<const Number &>(other).m_value *
    //        (type() == other.type());
    // RTTI required
    // return dynamic_cast<const Number &>(obj).m_value;
  }

  static thread_local Number result;
};

//
// Var
//

class var {
public:
  var() : m_obj(std::make_shared<None>()) {}
  var(const Object &obj) { m_obj = obj.clone(); }
  var(Number::number_t value) { m_obj = std::make_shared<Number>(value); }
  var(const var &other) : m_obj(other.m_obj) {}
  var &operator=(const var &other) {
    if (m_obj != other.m_obj)
      m_obj = other.m_obj;
    return *this;
  }
  var &operator=(const Object &obj) {
    m_obj = obj.clone();
    return *this;
  }
  ~var() = default;

  Object::Type type() const { return m_obj->type(); }
  const char *type_name() const { return m_obj->type_name(); }
  std::string to_string() const { return m_obj->to_string(); }

  var operator+(const var &other) { return {*m_obj + *other.m_obj}; }
  var operator+(const Object &obj) { return {*m_obj + obj}; }
  var &operator+=(const var &other) {
    *m_obj += *other.m_obj;
    return *this;
  }
  var &operator+=(const Object &obj) {
    *m_obj += obj;
    return *this;
  }

  var operator-(const var &other) { return {*m_obj - *other.m_obj}; }
  var operator-(const Object &obj) { return {*m_obj - obj}; }
  var &operator-=(const var &other) {
    *m_obj -= *other.m_obj;
    return *this;
  }
  var &operator-=(const Object &obj) {
    *m_obj -= obj;
    return *this;
  }

  var operator*(const var &other) { return {*m_obj * *other.m_obj}; }
  var operator*(const Object &obj) { return {*m_obj * obj}; }
  var &operator*=(const var &other) {
    *m_obj *= *other.m_obj;
    return *this;
  }
  var &operator*=(const Object &obj) {
    *m_obj *= obj;
    return *this;
  }

  var operator/(const var &other) { return {*m_obj / *other.m_obj}; }
  var operator/(const Object &obj) { return {*m_obj / obj}; }
  var &operator/=(const var &other) {
    *m_obj /= *other.m_obj;
    return *this;
  }
  var &operator/=(const Object &obj) {
    *m_obj /= obj;
    return *this;
  }

private:
  std::shared_ptr<Object> m_obj;
};

void print(const char *);
void print(const std::string &);
void print(Number::number_t);
void print(const Object &);
void print(const var &);

#endif