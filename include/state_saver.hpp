//   _____ _        _          _____                         _____
//  / ____| |      | |        / ____|                       / ____|_     _
// | (___ | |_ __ _| |_ ___  | (___   __ ___   _____ _ __  | |   _| |_ _| |_
//  \___ \| __/ _` | __/ _ \  \___ \ / _` \ \ / / _ \ '__| | |  |_   _|_   _|
//  ____) | || (_| | ||  __/  ____) | (_| |\ V /  __/ |    | |____|_|   |_|
// |_____/ \__\__,_|\__\___| |_____/ \__,_| \_/ \___|_|     \_____|
//
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

#ifndef NEARGYE_STATE_SAVER_HPP
#define NEARGYE_STATE_SAVER_HPP

// Sometimes a certain value has to change only for a limited scope.
// This class wrapper saves a copy of the current state of some object,
// and resets the object’s state at destruction time, undoing any change the object may have gone through.
// * saver_exit - saves the original variable value and restores on scope exit.
// * saver_fail - saves the original variable value and restores on scope exit when an exception has been thrown.
// * saver_success - saves the original variable value and restores on scope exit when no exceptions have been thrown.

// Interface of state_saver:
// * constructor state_saver(T& object) - construct state_saver with saved object.
// * dismiss() - dismiss restore on scope exit.
// * restore() - resets the object’s state. Requirements copy operator =.

// Requirements to saved object:
// * Object semantic (cannot be reference, function, ...).
// * Copy constructor.
// * operator= (no-throw one preferred).

// Throwable settings:
// STATE_SAVER_NO_THROW_CONSTRUCTIBLE requires nothrow constructible action.
// STATE_SAVER_MAY_THROW_RESTORE restore may throw exceptions.
// STATE_SAVER_NO_THROW_RESTORE requires noexcept restore.
// STATE_SAVER_SUPPRESS_THROW_RESTORE exceptions during restore will be suppressed.
// STATE_SAVER_CATCH_HANDLER exceptions handler. If STATE_SAVER_SUPPRESS_THROW_RESTORE is not defined, it will do nothing.

// Assignable settings:
// STATE_SAVER_FORCE_MOVE_ASSIGNABLE restore on scope exit will be move assigned.
// STATE_SAVER_FORCE_COPY_ASSIGNABLE restore on scope exit will be copy assigned.

#include <type_traits>
#if (defined(_MSC_VER) && _MSC_VER >= 1900) || ((defined(__clang__) || defined(__GNUC__)) && __cplusplus >= 201700L)
#include <exception>
#endif

#if !defined(STATE_SAVER_MAY_THROW_RESTORE) && !defined(STATE_SAVER_NO_THROW_RESTORE) && !defined(STATE_SAVER_SUPPRESS_THROW_RESTORE)
#  define STATE_SAVER_MAY_THROW_RESTORE
#elif (defined(STATE_SAVER_MAY_THROW_RESTORE) + defined(STATE_SAVER_NO_THROW_RESTORE) + defined(STATE_SAVER_SUPPRESS_THROW_RESTORE)) > 1
#  error Only one of STATE_SAVER_MAY_THROW_RESTORE and STATE_SAVER_NO_THROW_RESTORE and STATE_SAVER_SUPPRESS_THROW_RESTORE may be defined.
#endif

#if (defined(STATE_SAVER_FORCE_MOVE_ASSIGNABLE) + defined(STATE_SAVER_FORCE_COPY_ASSIGNABLE)) > 1
#  error Only one of STATE_SAVER_FORCE_MOVE_ASSIGNABLE and STATE_SAVER_FORCE_COPY_ASSIGNABLE may be defined.
#endif

#if !defined(STATE_SAVER_CATCH_HANDLER)
#  define STATE_SAVER_CATCH_HANDLER /* Suppress exception.*/
#endif

namespace nstd {

namespace detail {

#if defined(STATE_SAVER_SUPPRESS_THROW_RESTORE) && (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND))
#  define NEARGYE_NOEXCEPT(...) noexcept
#  define NEARGYE_TRY           try {
#  define NEARGYE_CATCH         } catch (...) { SCOPE_GUARD_CATCH_HANDLER }
#else
#  define NEARGYE_NOEXCEPT(...) noexcept(__VA_ARGS__)
#  define NEARGYE_TRY
#  define NEARGYE_CATCH
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
inline int uncaught_exceptions() noexcept {
  return *(reinterpret_cast<int*>(static_cast<char*>(static_cast<void*>(_getptd())) + (sizeof(void*) == 8 ? 0x100 : 0x90)));
}
#elif (defined(__clang__) || defined(__GNUC__)) && __cplusplus < 201700L
struct __cxa_eh_globals;
extern "C" __cxa_eh_globals* __cxa_get_globals() noexcept;
inline int uncaught_exceptions() noexcept {
  return static_cast<int>(*(reinterpret_cast<unsigned int*>(static_cast<char*>(static_cast<void*>(__cxa_get_globals())) + sizeof(void*))));
}
#else
inline int uncaught_exceptions() noexcept {
  return std::uncaught_exceptions();
}
#endif

class on_exit_policy {
  bool execute_;

 public:
  explicit on_exit_policy(bool execute) noexcept : execute_{execute} {}

  void dismiss() noexcept {
    execute_ = false;
  }

  bool should_execute() const noexcept {
    return execute_;
  }
};

class on_fail_policy {
  int ec_;

 public:
  explicit on_fail_policy(bool execute) noexcept : ec_{execute ? uncaught_exceptions() : -1} {}

  void dismiss() noexcept {
    ec_ = -1;
  }

  bool should_execute() const noexcept {
    return ec_ != -1 && ec_ < uncaught_exceptions();
  }
};

class on_success_policy {
  int ec_;

 public:
  explicit on_success_policy(bool execute) noexcept : ec_{execute ? uncaught_exceptions() : -1} {}

  void dismiss() noexcept {
    ec_ = -1;
  }

  bool should_execute() const noexcept {
    return ec_ != -1 && ec_ >= uncaught_exceptions();
  }
};

template <typename U, typename P>
class state_saver {
  using T = typename std::remove_reference<U>::type;
#if defined(STATE_SAVER_FORCE_MOVE_ASSIGNABLE)
  using assignable_t = T&&;
#elif defined(STATE_SAVER_FORCE_COPY_ASSIGNABLE)
  using assignable_t = T&;
#else
  using assignable_t = typename std::conditional<
      std::is_nothrow_assignable<T&, T&&>::value ||
          !std::is_assignable<T&, T&>::value ||
          (!std::is_nothrow_assignable<T&, T&>::value && std::is_assignable<T&, T&&>::value),
      T&&, T&>::type;
#endif

  static_assert(!std::is_const<T>::value,
                "state_saver requires not const type.");
  static_assert(!std::is_rvalue_reference<U>::value && (std::is_lvalue_reference<U>::value || std::is_same<T, U>::value),
                "state_saver requires lvalue type.");
  static_assert(!std::is_array<T>::value,
                "state_saver requires not array type.");
  static_assert(!std::is_pointer<T>::value,
                "state_saver requires not pointer type.");
  static_assert(!std::is_function<T>::value,
                "state_saver requires not function type.");
  static_assert(std::is_constructible<T, T&>::value,
                "state_saver requires copy constructible.");
  static_assert(std::is_assignable<T&, assignable_t>::value,
                "state_saver requires operator=.");
  static_assert(std::is_same<P, on_exit_policy>::value || std::is_same<P, on_fail_policy>::value || std::is_same<P, on_success_policy>::value,
                "state_saver requires on_exit_policy, on_fail_policy or on_success_policy.");
#if defined(STATE_SAVER_NO_THROW_RESTORE)
  static_assert(std::is_nothrow_assignable<T&, assignable_t>::value,
                "state_saver requires noexcept operator=.");
#endif
#if defined(STATE_SAVER_NO_THROW_CONSTRUCTIBLE)
  static_assert(std::is_nothrow_constructible<T, T&>::value,
                "state_saver requires nothrow constructible.");
#endif

  P policy_;
  T& previous_ref_;
  T previous_value_;

 public:
  state_saver() = delete;
  state_saver(const state_saver&) = delete;
  state_saver(state_saver&&) = delete;
  state_saver& operator=(const state_saver&) = delete;
  state_saver& operator=(state_saver&&) = delete;

  state_saver(T&&) = delete;
  state_saver(const T&) = delete;

  explicit state_saver(T& object) noexcept(std::is_nothrow_constructible<T, T&>::value)
      : policy_{true},
        previous_ref_{object},
        previous_value_{object} {}

  void dismiss() noexcept {
    policy_.dismiss();
  }

  template <typename O = T>
  auto restore() NEARGYE_NOEXCEPT(std::is_nothrow_assignable<O&, O&>::value) -> typename std::enable_if<std::is_same<T, O>::value && std::is_assignable<O&, O&>::value>::type {
    static_assert(std::is_assignable<O&, O&>::value, "state_saver::restore requires copy operator=.");
#if defined(STATE_SAVER_NO_THROW_RESTORE)
    static_assert(std::is_nothrow_assignable<O&, O&>::value, "state_saver::restore requires noexcept copy operator=.");
#endif
    NEARGYE_TRY
      previous_ref_ = previous_value_;
    NEARGYE_CATCH
  }

  ~state_saver() NEARGYE_NOEXCEPT(std::is_nothrow_assignable<T&, assignable_t>::value) {
    if (policy_.should_execute()) {
      NEARGYE_TRY
        previous_ref_ = static_cast<assignable_t>(previous_value_);
      NEARGYE_CATCH
    }
  }
};

#undef NEARGYE_NOEXCEPT
#undef NEARGYE_TRY
#undef NEARGYE_CATCH

} // namespace nstd::detail

template <typename U>
class saver_exit : public detail::state_saver<U, detail::on_exit_policy> {
 public:
  using detail::state_saver<U, detail::on_exit_policy>::state_saver;
};

template <typename U>
class saver_fail : public detail::state_saver<U, detail::on_fail_policy> {
 public:
  using detail::state_saver<U, detail::on_fail_policy>::state_saver;
};

template <typename U>
class saver_success : public detail::state_saver<U, detail::on_success_policy> {
 public:
  using detail::state_saver<U, detail::on_success_policy>::state_saver;
};

#if defined(__cpp_deduction_guides) && __cpp_deduction_guides >= 201611L
template <typename U>
saver_exit(U&) -> saver_exit<U>;

template <typename U>
saver_fail(U&) -> saver_fail<U>;

template <typename U>
saver_success(U&) -> saver_success<U>;
#endif

} // namespace nstd

// NEARGYE_MAYBE_UNUSED suppresses compiler warnings on unused entities, if any.
#if !defined(NEARGYE_MAYBE_UNUSED)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 39 && __cplusplus >= 201703L
#      define NEARGYE_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define NEARGYE_MAYBE_UNUSED __attribute__((__unused__))
#    endif
#  elif defined(__GNUC__)
#    if __GNUC__ >= 7 && __cplusplus >= 201703L
#      define NEARGYE_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define NEARGYE_MAYBE_UNUSED __attribute__((__unused__))
#    endif
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1911 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#      define NEARGYE_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define NEARGYE_MAYBE_UNUSED __pragma(warning(suppress : 4100 4101 4189))
#    endif
#  else
#    define NEARGYE_MAYBE_UNUSED
#  endif
#endif

#if !defined(NEARGYE_STR_CONCAT)
#  define NEARGYE_STR_CONCAT_(s1, s2) s1##s2
#  define NEARGYE_STR_CONCAT(s1, s2)  NEARGYE_STR_CONCAT_(s1, s2)
#endif

#if !defined(NEARGYE_COUNTER)
#  if defined(__COUNTER__)
#    define NEARGYE_COUNTER __COUNTER__
#  elif defined(__LINE__)
#    define NEARGYE_COUNTER __LINE__
#  endif
#endif

#define NEARGYE_STATE_SAVER_WITH_(s, i) for (int i = 1; i--; s)
#define NEARGYE_STATE_SAVER_WITH(s)     NEARGYE_STATE_SAVER_WITH_(s, NEARGYE_STR_CONCAT(NEARGYE_INTERNAL_OBJECT_, NEARGYE_COUNTER))

// SAVER_EXIT saves the original variable value and restores on scope exit.
#define MAKE_SAVER_EXIT(name, x) ::nstd::saver_exit<decltype(x)> name{x}
#define SAVER_EXIT(x)            NEARGYE_MAYBE_UNUSED const MAKE_SAVER_EXIT(NEARGYE_STR_CONCAT(SAVER_EXIT_, NEARGYE_COUNTER), x)
#define WITH_SAVER_EXIT(x)       NEARGYE_STATE_SAVER_WITH(::nstd::saver_exit<decltype(x)>{x})

// SAVER_FAIL saves the original variable value and restores on scope exit when an exception has been thrown.
#define MAKE_SAVER_FAIL(name, x) ::nstd::saver_fail<decltype(x)> name{x}
#define SAVER_FAIL(x)            NEARGYE_MAYBE_UNUSED const MAKE_SAVER_FAIL(NEARGYE_STR_CONCAT(SAVER_FAIL_, NEARGYE_COUNTER), x)
#define WITH_SAVER_FAIL(x)       NEARGYE_STATE_SAVER_WITH(::nstd::saver_fail<decltype(x)>{x})

// SAVER_SUCCESS saves the original variable value and restores on scope exit when no exceptions have been thrown.
#define MAKE_SAVER_SUCCESS(name, x) ::nstd::saver_success<decltype(x)> name{x}
#define SAVER_SUCCESS(x)            NEARGYE_MAYBE_UNUSED const MAKE_SAVER_SUCCESS(NEARGYE_STR_CONCAT(SAVER_SUCCES_, NEARGYE_COUNTER), x)
#define WITH_SAVER_SUCCESS(x)       NEARGYE_STATE_SAVER_WITH(::nstd::saver_success<decltype(x)>{x})

#endif // NEARGYE_STATE_SAVER_HPP
