#pragma once

namespace CppSerial {
class EndiannessProvider {
 public:
  enum class Endianness {
    LITTLE = 0,
    BIG = 1,
  };

  static Endianness GetSystemEndianness();
};
}  // namespace CppSerial
