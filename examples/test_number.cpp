#include "../src/cnake.h"
#include <iostream>
#include <string>

#define STRINGIFY_OP(op) #op

#define TEST_BIN_OP(op)                                                        \
  a = 7.0546f;                                                                 \
  b = 5;                                                                       \
  b = b;                                                                       \
  a = (b)op(true);                                                             \
  b = (a)op(b);                                                                \
                                                                               \
  a_correct = 7.0546f;                                                         \
  b_correct = 5;                                                               \
  b_correct = b_correct;                                                       \
  a_correct = (b_correct)op(true);                                             \
  b_correct = (a_correct)op(b_correct);                                        \
  std::cout << "Testing operator: '" << STRINGIFY_OP(op) << "'" << '\n';

#define TEST_COMP_OP(op)                                                       \
  a = 6.14567;                                                                 \
  b = -2;                                                                      \
  (b) op true;                                                                 \
  (a) op(b);                                                                   \
                                                                               \
  a_correct = 6.14567;                                                         \
  b_correct = -2;                                                              \
  (b_correct) op true;                                                         \
  (a_correct) op(b_correct);                                                   \
  std::cout << "Testing operator: '" << STRINGIFY_OP(op) << "'" << '\n';

namespace {

Number a;
Number b;
Number::number_t a_correct{};
Number::number_t b_correct{};

void print_a_b() {
  std::cout << "Number a: " << a.to_string() << " Number b: " << b.to_string()
            << '\n';
  std::cout << "Correct a: " << std::to_string(a_correct)
            << " Correct b: " << std::to_string(b_correct) << "\n\n";
}

} // namespace

int main() {
  std::cout << "Testing binary operators\n";
  TEST_BIN_OP(+);
  print_a_b();
  TEST_BIN_OP(-);
  print_a_b();
  TEST_BIN_OP(*);
  print_a_b();
  TEST_BIN_OP(/);
  print_a_b();

  std::cout << "Testing compound operators\n";
  TEST_COMP_OP(+=);
  print_a_b();
  TEST_COMP_OP(-=);
  print_a_b();
  TEST_COMP_OP(*=);
  print_a_b();
  TEST_COMP_OP(/=);
  print_a_b();

  std::cout << std::flush;
}