#include "cnake.h"
#include <cstdint>
#include <cstdio>
#include <iostream>

None None::failsafe_none{};
const uint64_t None::failsafe_null{};
thread_local Number Number::result{};

void print(const char *message) { std::cout << message << '\n'; }
void print(const std::string &message) { std::cout << message << '\n'; }
void print(Number::number_t message) { std::cout << message << '\n'; }
void print(const Object &obj) { std::cout << obj.to_string() << '\n'; }
void print(const var &var) { std::cout << var.to_string() << '\n'; }