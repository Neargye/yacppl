// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2024 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the"Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED"AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#undef TEST_CLASS_NAME
#define TEST_CLASS_NAME test_class_success

#include "state_saver_test_case.hpp"

CASE_TEST("state_saver_success: called on scope leave") {
  test_class a{test_value};
  const auto some_function = [](test_class& a) {
    saver_success<decltype(a)> saver_success{a};
    a.i = other_test_value;
    REQUIRE(a.i == other_test_value);
  };

  REQUIRE_NOTHROW([&]() {
    some_function(a);
  }());

  REQUIRE(a.i == test_value);
}

CASE_TEST("state_saver_success: not called on error") {
  test_class a{test_value};
  const auto some_function = [](test_class& a) {
    saver_success<decltype(a)> saver_success{a};
    a.i = other_test_value;
    REQUIRE(a.i == other_test_value);
    throw std::runtime_error{"error"};
  };

  REQUIRE_THROWS([&]() {
    some_function(a);
  }());

  REQUIRE(a.i == other_test_value);
}

CASE_TEST("state_saver_success: dismiss before scope leave") {
  test_class a{test_value};
  const auto some_function = [](test_class& a) {
    saver_success<decltype(a)> saver_success{a};
    a.i = other_test_value;
    REQUIRE(a.i == other_test_value);
    saver_success.dismiss();
  };

  REQUIRE_NOTHROW([&]() {
    some_function(a);
  }());

  REQUIRE(a.i == other_test_value);
}

CASE_TEST("state_saver_success: dismiss before error") {
  test_class a{test_value};
  const auto some_function = [](test_class& a) {
    saver_success<decltype(a)> saver_success{a};
    a.i = other_test_value;
    REQUIRE(a.i == other_test_value);
    saver_success.dismiss();
    throw std::runtime_error{"error"};
  };

  REQUIRE_THROWS([&]() {
    some_function(a);
  }());

  REQUIRE(a.i == other_test_value);
}

CASE_TEST("state_saver_success: not called on error, dismiss after error") {
  test_class a{test_value};
  const auto some_function = [](test_class& a) {
    saver_success<decltype(a)> saver_success{a};
    a.i = other_test_value;
    REQUIRE(a.i == other_test_value);
    throw std::runtime_error{"error"};
    saver_success.dismiss();
  };

  REQUIRE_THROWS([&]() {
    some_function(a);
  }());

  REQUIRE(a.i == other_test_value);
}

#if CASE_NUMBER != 3
CASE_TEST("state_saver_success: restore") {
  test_class a{test_value};
  const auto some_function = [](test_class& a) {
    saver_success<decltype(a)> saver_success{a};
    a.i = other_test_value;
    REQUIRE(a.i == other_test_value);
    saver_success.restore();
    REQUIRE(a.i == test_value);
    a.i = other_test_value;
  };

  REQUIRE_NOTHROW_IF(is_nothrow_restore, [&]() {
    some_function(a);
  }());

  if (is_nothrow_restore) {
    REQUIRE(a.i == test_value);
  } else {
    REQUIRE(a.i == other_test_value);
  }
}

CASE_TEST("state_saver_success: dismiss, restore") {
  test_class a{test_value};
  const auto some_function = [](test_class& a) {
    saver_success<decltype(a)> saver_success{a};
    a.i = other_test_value;
    REQUIRE(a.i == other_test_value);
    saver_success.dismiss();
    saver_success.restore();
    REQUIRE(a.i == test_value);
    a.i = other_test_value;
  };

  REQUIRE_NOTHROW_IF(is_nothrow_restore, [&]() {
    some_function(a);
  }());

  REQUIRE(a.i == other_test_value);
}
#endif
