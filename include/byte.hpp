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
#include <type_traits>

namespace nstd {

enum class byte : unsigned char {};

namespace detail {

template <typename I>
struct is_byte_shift_count
    : std::integral_constant<bool, std::is_integral<I>::value && !std::is_same<typename std::remove_cv<I>::type, bool>::value> {};

template <typename I>
constexpr auto is_valid_byte_shift(I shift) noexcept -> std::enable_if_t<std::is_signed<I>::value, bool> {
  return shift >= 0 && static_cast<unsigned long long>(shift) < static_cast<unsigned long long>(std::numeric_limits<unsigned int>::digits);
}

template <typename I>
constexpr auto is_valid_byte_shift(I shift) noexcept -> std::enable_if_t<!std::is_signed<I>::value, bool> {
  return static_cast<unsigned long long>(shift) < static_cast<unsigned long long>(std::numeric_limits<unsigned int>::digits);
}

} // namespace detail

template <typename I>
[[nodiscard]] constexpr auto to_byte(I value) noexcept -> std::enable_if_t<std::is_integral_v<I>, byte> {
  return static_cast<byte>(value);
}

template <typename I = unsigned char>
[[nodiscard]] constexpr auto to_integer(byte b) noexcept -> std::enable_if_t<std::is_integral_v<I>, I> {
  return static_cast<I>(b);
}

[[nodiscard]] constexpr byte operator~(byte b) noexcept {
  return static_cast<byte>(static_cast<unsigned char>(~static_cast<unsigned int>(b)));
}

[[nodiscard]] constexpr byte operator|(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs)));
}

[[nodiscard]] constexpr byte operator&(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs)));
}

[[nodiscard]] constexpr byte operator^(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs)));
}

template <typename I>
[[nodiscard]] constexpr auto operator<<(byte b, I shift) noexcept -> std::enable_if_t<detail::is_byte_shift_count<I>::value, byte> {
  assert(detail::is_valid_byte_shift(shift) && "nstd::byte shift out of range");
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(b) << static_cast<unsigned int>(shift)));
}

template <typename I>
[[nodiscard]] constexpr auto operator>>(byte b, I shift) noexcept -> std::enable_if_t<detail::is_byte_shift_count<I>::value, byte> {
  assert(detail::is_valid_byte_shift(shift) && "nstd::byte shift out of range");
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(b) >> static_cast<unsigned int>(shift)));
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
auto to_bytes(byte* dst, const T& src) noexcept -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::to_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable_v<T>,    "nstd::to_bytes requires T is trivially copyable.");
  assert(dst != nullptr && "nstd::to_bytes requires dst is not null");
  static_cast<void>(std::memcpy(dst, &src, sizeof(T)));
}

template <typename T>
auto to_bytes(byte* dst, const T* src, std::size_t count) noexcept -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::to_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable_v<T>,    "nstd::to_bytes requires T is trivially copyable.");
  assert(dst != nullptr && "nstd::to_bytes requires dst is not null");
  assert(src != nullptr && "nstd::to_bytes requires src is not null");
  assert(count <= std::numeric_limits<std::size_t>::max() / sizeof(T) && "nstd::to_bytes count overflow");
  static_cast<void>(std::memcpy(dst, src, count * sizeof(T)));
}

template <typename T, std::size_t N>
auto to_bytes(byte* dst, const T (&src)[N]) noexcept -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  return to_bytes(dst, src, N);
}

template <typename T>
[[nodiscard]] auto from_bytes(const byte* src) noexcept(std::is_nothrow_default_constructible_v<T>) -> std::enable_if_t<std::is_trivially_copyable_v<T> && std::is_default_constructible_v<T>, T> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable_v<T>,    "nstd::from_bytes requires T is trivially copyable.");
  static_assert(std::is_default_constructible_v<T>, "nstd::from_bytes requires T is default constructible.");
  assert(src != nullptr && "nstd::from_bytes requires src is not null");
  T dst;
  static_cast<void>(std::memcpy(&dst, src, sizeof(T)));
  return dst;
}

template <typename T>
auto from_bytes(T& dst, const byte* src) noexcept -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable_v<T>,    "nstd::from_bytes requires T is trivially copyable.");
  assert(src != nullptr && "nstd::from_bytes requires src is not null");
  static_cast<void>(std::memcpy(&dst, src, sizeof(T)));
}

template <typename T>
auto from_bytes(T* dst, const byte* src, std::size_t count) noexcept -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable_v<T>,    "nstd::from_bytes requires T is trivially copyable.");
  assert(dst != nullptr && "nstd::from_bytes requires dst is not null");
  assert(src != nullptr && "nstd::from_bytes requires src is not null");
  assert(count <= std::numeric_limits<std::size_t>::max() / sizeof(T) && "nstd::from_bytes count overflow");
  static_cast<void>(std::memcpy(dst, src, count * sizeof(T)));
}

template <typename T, std::size_t N>
auto from_bytes(T (&dst)[N], const byte* src) noexcept -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  return from_bytes(dst, src, N);
}

} // namespace nstd

#endif // NEARGYE_NSTD_BYTE_HPP
