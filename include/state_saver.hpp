//   _____ _        _          _____                         _____
//  / ____| |      | |        / ____|                       / ____|_     _
// | (___ | |_ __ _| |_ ___  | (___   __ ___   _____ _ __  | |   _| |_ _| |_
//  \___ \| __/ _` | __/ _ \  \___ \ / _` \ \ / / _ \ '__| | |  |_   _|_   _|
//  ____) | || (_| | ||  __/  ____) | (_| |\ V /  __/ |    | |____|_|   |_|
// |_____/ \__\__,_|\__\___| |_____/ \__,_| \_/ \___|_|     \_____|
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.
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

#ifndef NEARGYE_NSTD_STATE_SAVER_HPP
#define NEARGYE_NSTD_STATE_SAVER_HPP

// state_saver saves an object's current value and restores it later.
// * saver_exit - restores on scope exit.
// * saver_fail - restores on scope exit when a new exception is being unwound.
// * saver_success - restores on scope exit when no new exception is being unwound.

// Interface:
// * state_saver(T& object) - saves the current object value.
// * dismiss() - disables automatic restore on scope exit.
// * restore() - restores the saved value immediately. Requires lvalue assignment.

// Requirements:
// * Object semantics (non-const, non-pointer, non-array, non-function).
// * Constructible from a non-const lvalue.
// * Nothrow destructible.
// * Assignment operator selected by the restore policy.

// Define settings before including this header and use the same values in every translation unit.

// Restore settings:
// STATE_SAVER_NO_THROW_CONSTRUCTIBLE requires nothrow snapshot construction.
// STATE_SAVER_MAY_THROW_RESTORE allows restore to throw exceptions.
// STATE_SAVER_NO_THROW_RESTORE requires noexcept restore.
// STATE_SAVER_SUPPRESS_THROW_RESTORE suppresses exceptions thrown during restore.
// STATE_SAVER_CATCH_HANDLER handles suppressed restore exceptions and must not throw.

// Assignment settings:
// STATE_SAVER_FORCE_MOVE_ASSIGNABLE uses rvalue assignment on scope exit.
// STATE_SAVER_FORCE_COPY_ASSIGNABLE uses lvalue assignment on scope exit.

#include <exception>
#include <type_traits>

#if !defined(_MSC_VER) && __cplusplus < 201703L && defined(__GXX_ABI_VERSION) && (defined(__clang__) || defined(__GNUC__))
namespace __cxxabiv1 {
struct __cxa_eh_globals;
extern "C" __cxa_eh_globals* __cxa_get_globals() noexcept;
} // namespace __cxxabiv1
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
#  define NEARGYE_STATE_SAVER_NOEXCEPT(...) noexcept
#  define NEARGYE_STATE_SAVER_TRY           try {
#  define NEARGYE_STATE_SAVER_CATCH         } catch (...) { STATE_SAVER_CATCH_HANDLER }
#else
#  define NEARGYE_STATE_SAVER_NOEXCEPT(...) noexcept(__VA_ARGS__)
#  define NEARGYE_STATE_SAVER_TRY
#  define NEARGYE_STATE_SAVER_CATCH
#endif

#if (defined(_MSC_VER) && _MSC_VER >= 1900) || __cplusplus >= 201703L
inline int uncaught_exceptions() noexcept {
  return std::uncaught_exceptions();
}
#elif defined(__GXX_ABI_VERSION) && (defined(__clang__) || defined(__GNUC__))
inline int uncaught_exceptions() noexcept {
  return static_cast<int>(*reinterpret_cast<const unsigned int*>(reinterpret_cast<const char*>(::__cxxabiv1::__cxa_get_globals()) + sizeof(void*)));
}
#else
#  error state_saver requires std::uncaught_exceptions() or a GCC-compatible Itanium C++ ABI.
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
  using assignable_t = typename std::conditional<!std::is_nothrow_assignable<T&, T&&>::value && std::is_assignable<T&, T&>::value, T&, T&&>::type;
#endif

#if defined(STATE_SAVER_NO_THROW_RESTORE)
  template <typename O>
  using is_restore_assignable = std::integral_constant<bool, std::is_same<T, O>::value && std::is_nothrow_assignable<O&, O&>::value>;
#else
  template <typename O>
  using is_restore_assignable = std::integral_constant<bool, std::is_same<T, O>::value && std::is_assignable<O&, O&>::value>;
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
                "state_saver requires constructible from an lvalue.");
  static_assert(std::is_nothrow_destructible<T>::value,
                "state_saver requires nothrow destructible type.");
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
                "state_saver requires nothrow constructible from an lvalue.");
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
      : policy_{true}, previous_ref_{object}, previous_value_(object) {}

  void dismiss() noexcept {
    policy_.dismiss();
  }

  template <typename O = T>
  auto restore() NEARGYE_STATE_SAVER_NOEXCEPT(std::is_nothrow_assignable<O&, O&>::value) -> typename std::enable_if<is_restore_assignable<O>::value>::type {
    NEARGYE_STATE_SAVER_TRY
      previous_ref_ = previous_value_;
    NEARGYE_STATE_SAVER_CATCH
  }

  ~state_saver() NEARGYE_STATE_SAVER_NOEXCEPT(std::is_nothrow_assignable<T&, assignable_t>::value) {
    if (policy_.should_execute()) {
      NEARGYE_STATE_SAVER_TRY
        previous_ref_ = static_cast<assignable_t>(previous_value_);
      NEARGYE_STATE_SAVER_CATCH
    }
  }
};

#undef NEARGYE_STATE_SAVER_NOEXCEPT
#undef NEARGYE_STATE_SAVER_TRY
#undef NEARGYE_STATE_SAVER_CATCH

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

// NEARGYE_STATE_SAVER_MAYBE_UNUSED suppresses compiler warnings on unused entities, if any.
#if !defined(NEARGYE_STATE_SAVER_MAYBE_UNUSED)
#  if defined(__clang__)
#    if (__clang_major__ * 10 + __clang_minor__) >= 39 && __cplusplus >= 201703L
#      define NEARGYE_STATE_SAVER_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define NEARGYE_STATE_SAVER_MAYBE_UNUSED __attribute__((__unused__))
#    endif
#  elif defined(__GNUC__)
#    if __GNUC__ >= 7 && __cplusplus >= 201703L
#      define NEARGYE_STATE_SAVER_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define NEARGYE_STATE_SAVER_MAYBE_UNUSED __attribute__((__unused__))
#    endif
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1911 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#      define NEARGYE_STATE_SAVER_MAYBE_UNUSED [[maybe_unused]]
#    else
#      define NEARGYE_STATE_SAVER_MAYBE_UNUSED __pragma(warning(suppress : 4100 4101 4189))
#    endif
#  else
#    define NEARGYE_STATE_SAVER_MAYBE_UNUSED
#  endif
#endif

#if !defined(NEARGYE_STATE_SAVER_STR_CONCAT)
#  define NEARGYE_STATE_SAVER_STR_CONCAT_(s1, s2) s1##s2
#  define NEARGYE_STATE_SAVER_STR_CONCAT(s1, s2)  NEARGYE_STATE_SAVER_STR_CONCAT_(s1, s2)
#endif

#if !defined(NEARGYE_STATE_SAVER_COUNTER)
#  if defined(__COUNTER__)
#    define NEARGYE_STATE_SAVER_COUNTER __COUNTER__
#  elif defined(__LINE__)
#    define NEARGYE_STATE_SAVER_COUNTER __LINE__
#  endif
#endif

#define NEARGYE_STATE_SAVER_WITH_(s, x, g, i) for (bool i = true; i; i = false) for (NEARGYE_STATE_SAVER_MAYBE_UNUSED s g{x}; i; i = false)
#define NEARGYE_STATE_SAVER_WITH(s, x)        NEARGYE_STATE_SAVER_WITH_(s, x, NEARGYE_STATE_SAVER_STR_CONCAT(NEARGYE_STATE_SAVER_INTERNAL_OBJECT_, NEARGYE_STATE_SAVER_COUNTER), NEARGYE_STATE_SAVER_STR_CONCAT(NEARGYE_STATE_SAVER_INTERNAL_FLAG_, NEARGYE_STATE_SAVER_COUNTER))

// SAVER_EXIT saves the original variable value and restores on scope exit.
#define MAKE_SAVER_EXIT(name, x) ::nstd::saver_exit<decltype(x)> name{x}
#define SAVER_EXIT(x)            NEARGYE_STATE_SAVER_MAYBE_UNUSED const MAKE_SAVER_EXIT(NEARGYE_STATE_SAVER_STR_CONCAT(SAVER_EXIT_, NEARGYE_STATE_SAVER_COUNTER), x)
#define WITH_SAVER_EXIT(x)       NEARGYE_STATE_SAVER_WITH(::nstd::saver_exit<decltype(x)>, x)

// SAVER_FAIL saves the original variable value and restores on scope exit when a new exception is being unwound.
#define MAKE_SAVER_FAIL(name, x) ::nstd::saver_fail<decltype(x)> name{x}
#define SAVER_FAIL(x)            NEARGYE_STATE_SAVER_MAYBE_UNUSED const MAKE_SAVER_FAIL(NEARGYE_STATE_SAVER_STR_CONCAT(SAVER_FAIL_, NEARGYE_STATE_SAVER_COUNTER), x)
#define WITH_SAVER_FAIL(x)       NEARGYE_STATE_SAVER_WITH(::nstd::saver_fail<decltype(x)>, x)

// SAVER_SUCCESS saves the original variable value and restores on scope exit when no new exception is being unwound.
#define MAKE_SAVER_SUCCESS(name, x) ::nstd::saver_success<decltype(x)> name{x}
#define SAVER_SUCCESS(x)            NEARGYE_STATE_SAVER_MAYBE_UNUSED const MAKE_SAVER_SUCCESS(NEARGYE_STATE_SAVER_STR_CONCAT(SAVER_SUCCESS_, NEARGYE_STATE_SAVER_COUNTER), x)
#define WITH_SAVER_SUCCESS(x)       NEARGYE_STATE_SAVER_WITH(::nstd::saver_success<decltype(x)>, x)

#endif // NEARGYE_NSTD_STATE_SAVER_HPP
