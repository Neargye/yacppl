//  _   _  _____ _______ _____     _____
// | \ | |/ ____|__   __|  __ \   / ____|_     _
// |  \| | (___    | |  | |  | | | |   _| |_ _| |_
// | . ` |\___ \   | |  | |  | | | |  |_   _|_   _|
// | |\  |____) |  | |  | |__| | | |____|_|   |_|
// |_| \_|_____/   |_|  |_____/   \_____|
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2020 Daniil Goncharov <neargye@gmail.com>.
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

#include <cstring>
#include <type_traits>

namespace nstd {

enum class byte : unsigned char {};

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
[[nodiscard]] constexpr auto operator<<(byte& b, I shift) noexcept -> std::enable_if_t<std::is_integral_v<I>, byte> {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(b) << shift));
}

template <typename I>
[[nodiscard]] constexpr auto operator>>(byte& b, I shift) noexcept -> std::enable_if_t<std::is_integral_v<I>, byte> {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(b) >> shift));
}

[[nodiscard]] constexpr byte& operator|=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs | rhs;
}

[[nodiscard]] constexpr byte& operator&=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs & rhs;
}

[[nodiscard]] constexpr byte& operator^=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs ^ rhs;
}

template <typename I>
[[nodiscard]] constexpr auto operator<<=(byte& b, I shift) noexcept -> std::enable_if_t<std::is_integral_v<I>, byte&> {
  return b = b << shift;
}

template <typename I>
[[nodiscard]] constexpr auto operator>>=(byte& b, I shift) noexcept -> std::enable_if_t<std::is_integral_v<I>, byte&> {
  return b = b >> shift;
}

template <typename T>
auto to_bytes(byte* dst, const T& src) -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable_v<T>,    "nstd::to_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, &src, sizeof(T)));
}

template <typename T>
auto to_bytes(byte* dst, const T* src, std::size_t count) -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable_v<T>,    "nstd::to_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, src, count));
}

template <typename T>
[[nodiscard]] auto from_bytes(const byte* src) -> std::enable_if_t<std::is_trivial_v<T>, T> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivial_v<T>,               "nstd::from_bytes requires T is trivial.");
  T dst;
  static_cast<void>(std::memcpy(&dst, src, sizeof(T)));
  return dst;
}

template <typename T>
[[nodiscard]] auto from_bytes(T* dst, const byte* src, std::size_t count) -> std::enable_if_t<std::is_trivially_copyable_v<T>> {
  static_assert(std::is_trivially_copyable_v<byte>, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable_v<T>,    "nstd::from_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, src, count));
}

} // namespace nstd

#endif // NEARGYE_NSTD_BYTE_HPP
