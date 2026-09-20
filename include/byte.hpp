//  _   _  _____ _______ _____     _____
// | \ | |/ ____|__   __|  __ \   / ____|_     _
// |  \| | (___    | |  | |  | | | |   _| |_ _| |_
// | . ` |\___ \   | |  | |  | | | |  |_   _|_   _|
// | |\  |____) |  | |  | |__| | | |____|_|   |_|
// |_| \_|_____/   |_|  |_____/   \_____|
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

#ifndef NEARGYE_NSTD_BYTE_HPP
#define NEARGYE_NSTD_BYTE_HPP

#include <cassert>
#include <cstddef>
#include <cstring>
#include <limits>
#include <memory>
#include <type_traits>

namespace nstd {

enum class byte : unsigned char {};

namespace detail {

template <typename I>
using is_byte_shift_count = std::bool_constant<std::is_integral_v<I> && !std::is_same_v<std::remove_cv_t<I>, bool>>;

template <typename I>
constexpr bool is_valid_byte_shift(I shift) noexcept {
  if constexpr (std::is_signed_v<I>) {
    if (shift < 0) {
      return false;
    }
  }
  return static_cast<unsigned long long>(shift) < static_cast<unsigned long long>(std::numeric_limits<unsigned int>::digits);
}

template <typename T>
using enable_if_byte_source_t = std::enable_if_t<std::is_trivially_copyable_v<T> && !std::is_volatile_v<T>>;

template <typename T>
using enable_if_byte_destination_t = std::enable_if_t<std::is_trivially_copyable_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T>>;

template <typename T>
bool can_copy_bytes(void* dst, const void* src, std::size_t count) noexcept {
  if (count == 0) {
    return false;
  }
  const bool valid_count = count <= (std::numeric_limits<std::size_t>::max)() / sizeof(T);
  assert(valid_count && "nstd byte copy count overflow");
  assert(dst != nullptr && "nstd byte copy requires dst is not null");
  assert(src != nullptr && "nstd byte copy requires src is not null");
  return valid_count && dst != nullptr && src != nullptr;
}

template <typename T>
using byte_return_source_t = std::conditional_t<std::is_trivially_copy_constructible_v<T>, const T&, T&&>;

template <typename T>
using enable_if_byte_return_t = std::enable_if_t<std::is_trivially_copyable_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T> &&
                                                std::is_default_constructible_v<T> && std::is_trivially_constructible_v<T, byte_return_source_t<T>>, T>;

} // namespace nstd::detail

template <typename I>
[[nodiscard]] constexpr auto to_byte(I value) noexcept -> std::enable_if_t<std::is_integral_v<I>, byte> {
  return static_cast<byte>(value);
}

template <typename I = unsigned char>
[[nodiscard]] constexpr auto to_integer(byte b) noexcept -> std::enable_if_t<std::is_integral_v<I>, I> {
  return static_cast<I>(b);
}

[[nodiscard]] constexpr byte operator~(byte b) noexcept {
  return static_cast<byte>(~static_cast<unsigned int>(b));
}

[[nodiscard]] constexpr byte operator|(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr byte operator&(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

[[nodiscard]] constexpr byte operator^(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs));
}

template <typename I>
[[nodiscard]] constexpr auto operator<<(byte b, I shift) noexcept -> std::enable_if_t<detail::is_byte_shift_count<I>::value, byte> {
  assert(detail::is_valid_byte_shift(shift) && "nstd::byte shift out of range");
  return static_cast<byte>(static_cast<unsigned int>(b) << static_cast<unsigned int>(shift));
}

template <typename I>
[[nodiscard]] constexpr auto operator>>(byte b, I shift) noexcept -> std::enable_if_t<detail::is_byte_shift_count<I>::value, byte> {
  assert(detail::is_valid_byte_shift(shift) && "nstd::byte shift out of range");
  return static_cast<byte>(static_cast<unsigned int>(b) >> static_cast<unsigned int>(shift));
}

constexpr byte& operator|=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs | rhs;
}

constexpr byte& operator&=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs & rhs;
}

constexpr byte& operator^=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs ^ rhs;
}

template <typename I>
constexpr auto operator<<=(byte& b, I shift) noexcept -> std::enable_if_t<detail::is_byte_shift_count<I>::value, byte&> {
  return b = b << shift;
}

template <typename I>
constexpr auto operator>>=(byte& b, I shift) noexcept -> std::enable_if_t<detail::is_byte_shift_count<I>::value, byte&> {
  return b = b >> shift;
}

template <typename T>
auto to_bytes(byte* dst, const T* src, std::size_t count) noexcept -> detail::enable_if_byte_source_t<T> {
  if (detail::can_copy_bytes<T>(dst, src, count)) {
    static_cast<void>(std::memmove(dst, src, count * sizeof(T)));
  }
}

template <typename T>
auto to_bytes(byte* dst, const T& src) noexcept -> detail::enable_if_byte_source_t<T> {
  return ::nstd::to_bytes(dst, std::addressof(src), 1);
}

template <typename T, std::size_t N>
auto to_bytes(byte* dst, const T (&src)[N]) noexcept -> detail::enable_if_byte_source_t<T> {
  return ::nstd::to_bytes(dst, src, N);
}

template <typename T>
[[nodiscard]] auto from_bytes(const byte* src) noexcept(std::is_nothrow_default_constructible_v<T> && std::is_nothrow_constructible_v<T, detail::byte_return_source_t<T>>) -> detail::enable_if_byte_return_t<T> {
  assert(src != nullptr && "nstd::from_bytes requires src is not null");
  T dst{};
  if (src != nullptr) {
    static_cast<void>(std::memcpy(static_cast<void*>(std::addressof(dst)), src, sizeof(T)));
  }
  return T(static_cast<detail::byte_return_source_t<T>>(dst));
}

template <typename T>
auto from_bytes(T* dst, const byte* src, std::size_t count) noexcept -> detail::enable_if_byte_destination_t<T> {
  if (detail::can_copy_bytes<T>(dst, src, count)) {
    static_cast<void>(std::memmove(static_cast<void*>(dst), src, count * sizeof(T)));
  }
}

template <typename T>
auto from_bytes(T& dst, const byte* src) noexcept -> detail::enable_if_byte_destination_t<T> {
  return ::nstd::from_bytes(std::addressof(dst), src, 1);
}

template <typename T, std::size_t N>
auto from_bytes(T (&dst)[N], const byte* src) noexcept -> detail::enable_if_byte_destination_t<T> {
  return ::nstd::from_bytes(dst, src, N);
}

} // namespace nstd

#endif // NEARGYE_NSTD_BYTE_HPP
