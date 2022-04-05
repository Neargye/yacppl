// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2022 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <utility>
#include <stdexcept>
#include <string>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <state_saver.hpp>
using namespace nstd;

#if !defined(CASE_TEST)
#  define CASE_TEST(name) TEST_CASE(std::string(name).append(" ").append(std::to_string(CASE_NUMBER)))
#endif

#if !defined(test_class)
#  define test_class NEARGYE_STR_CONCAT(TEST_CLASS_NAME, CASE_NUMBER)
#endif

class test_class {
 public:
  int i = 0;

  explicit test_class(int i) : i{i} {}
  test_class() = delete;
  ~test_class() = default;
  test_class(test_class&&) = default;
  test_class(const test_class&) = default;

#if CASE_NUMBER == 1
  test_class& operator=(const test_class&) = default;
  test_class& operator=(test_class&&) = default;
#elif CASE_NUMBER == 2
  test_class& operator=(const test_class&) = default;
  test_class& operator=(test_class&&) = delete;
#elif CASE_NUMBER == 3
  test_class& operator=(const test_class&) = delete;
  test_class& operator=(test_class&&) = default;
#elif CASE_NUMBER == 4
  test_class& operator=(const test_class&) noexcept(false) {
    throw std::runtime_error{"operator=(const test_class&) throw."};
  }
  test_class& operator=(test_class&&) = default;
#elif CASE_NUMBER == 5
  test_class& operator=(const test_class&) = default;
  test_class& operator=(test_class&&) noexcept(false) {
    throw std::runtime_error{"operator=(test_class&&) throw."};
  }
#endif
};

#if !defined(test_value)
#  define test_value 1
#endif

#if !defined(other_test_value)
#  define other_test_value -1
#endif

#if !defined(is_nothrow_restore)
#  define is_nothrow_restore std::is_nothrow_assignable<test_class&, test_class&>::value
#endif

#if !defined(REQUIRE_NOTHROW_IF)
#  define REQUIRE_NOTHROW_IF(is_nothrow, ...) \
    if (is_nothrow) {                         \
      REQUIRE_NOTHROW(__VA_ARGS__);           \
    } else {                                  \
      REQUIRE_THROWS(__VA_ARGS__);            \
    }
#endif
