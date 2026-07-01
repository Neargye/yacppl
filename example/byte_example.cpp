// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <byte.hpp>

#include <array>
#include <iostream>

struct packet_header {
  unsigned short id;
  unsigned short size;
};

int main() {
  nstd::byte flags = nstd::to_byte(0x01);
  flags |= nstd::to_byte(0x04);
  flags <<= 1;

  const packet_header header{7, 128};
  std::array<nstd::byte, sizeof(packet_header)> bytes{};
  nstd::to_bytes(bytes.data(), header);

  const auto restored = nstd::from_bytes<packet_header>(bytes.data());

  std::cout << "flags = " << nstd::to_integer<unsigned int>(flags) << '\n';
  std::cout << "header id = " << restored.id << ", size = " << restored.size << '\n';

  return 0;
}
