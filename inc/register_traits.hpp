#include <cstdint>
#include <type_traits>

template <typename T>
struct DependentFalse : std::false_type {};

/**
 * Register size definitions
 */
template <unsigned int Size>
struct RegisterTraits {};

template <>
struct RegisterTraits<8> {
  using type = std::uint8_t;
};

template <>
struct RegisterTraits<16> {
  using type = std::uint16_t;
};

template <>
struct RegisterTraits<32> {
  using type = std::uint32_t;
};

template <>
struct RegisterTraits<64> {
  using type = std::uint64_t;
};

/**
 *  Register access definitions
 */
class Read {};

class Write {};

class Reserved {};

class ReadWrite : public Read, public Write {};