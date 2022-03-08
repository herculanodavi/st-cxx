#pragma once

#include <bitset>
#include <cstdint>
#include <limits>
#include <type_traits>

/**
 * This code is basically a copy of
 * https://github.com/SJSU-Dev2/libxbitset/blob/main/include/libxbitset/bitset.hpp
 * It was rewritten here so I could understand it step by step.
 */

namespace utils {
static constexpr uint8_t BITS_PER_BYTE = 8;

struct bitrange {
  uint32_t position;
  uint32_t width;

  /**
   *  Compile-time factory method
   */
  template <std::size_t BitPosition1, std::size_t BitPosition2>
  static consteval bitrange from() {
    constexpr uint32_t plus_one = 1;
    if constexpr (BitPosition1 < BitPosition2) {
      return bitrange{.position = BitPosition1,
                      .width = plus_one + (BitPosition2 - BitPosition1)};
    } else {
      return bitrange{.position = BitPosition2,
                      .width = plus_one + (BitPosition1 - BitPosition2)};
    }
  }

  /**
   *  Single bit factory method
   */
  template <std::size_t BitPosition>
  static constexpr bitrange from() {
    return bitrange{.position = BitPosition, .width = 1};
  }

  /**
   * The mask starting in origin
   */
  template <typename T>
  constexpr auto origin_mask() const {
    using UnsignedT = typename std::make_unsigned<T>::type;
    constexpr UnsignedT all_ones = std::numeric_limits<UnsignedT>::max();
    constexpr std::size_t target_width = sizeof(T) * BITS_PER_BYTE;

    return static_cast<UnsignedT>(all_ones >> (target_width - width));
  }

  /**
   * The mask in the given position
   */
  template <typename T>
  constexpr auto mask() const {
    // Need to use an unsigned version of the type T for the mask to make sure
    // that the shift right doesn't result in a sign extended shift.
    using UnsignedT = decltype(origin_mask<T>());

    return static_cast<UnsignedT>(origin_mask<T>() << position);
  }

  /**
   * The complement of the mask with the given position
   */
  template <typename T>
  constexpr auto inverted_mask() const {
    return ~mask<T>();
  }

  constexpr bool operator==(const bitrange& other) const {
    return other.position == position && other.width == width;
  }
};

/**
 * Class that extends bitset functionality
 */
template <typename T>
class bitset {
 public:
  static constexpr size_t bit_width = sizeof(T) * BITS_PER_BYTE;

  explicit bitset(T initial_value) : bitset_(initial_value) {}

  auto& set(std::size_t pos, bool value = true) {
    bitset_.set(pos, value);
    return *this;
  }

  auto& set(utils::bitrange p_range) {
    set(p_range.position);
    return *this;
  }

  auto& reset(std::size_t pos) {
    bitset_.reset(pos);
    return *this;
  }

  auto& reset(utils::bitrange p_range) {
    this->reset(p_range.position);
    return *this;
  }

  auto& flip(std::size_t pos) {
    bitset_.flip(pos);
    return *this;
  }

  auto& flip(utils::bitrange p_range) {
    this->flip(p_range.position);
    return *this;
  }

  auto test(std::size_t pos) const { return bitset_.test(pos); }

  auto test(utils::bitrange p_range) const {
    return this->test(p_range.position);
  }

  template <typename U>
  constexpr auto& insert(const bitrange& field, U value) {
    using NormalT = std::remove_volatile_t<T>;
    auto kBitmask = field.mask<std::remove_volatile_t<T>>();
    auto kInvertedBitmask = field.inverted_mask<std::remove_volatile_t<T>>();

    // Clear width's number of bits in the target value at the bit position
    // specified.
    bitset_ &= kInvertedBitmask;

    // AND value with mask to remove any bits beyond the specified width.
    // Shift masked value into bit position and OR with target value.
    bitset_ |= (static_cast<NormalT>(value) << field.position) & kBitmask;

    return *this;
  }

  template <bitrange field, typename U>
  constexpr auto& insert(U value) {
    using NormalT = std::remove_volatile_t<T>;
    auto kBitmask = field.mask<std::remove_volatile_t<T>>();
    auto kInvertedBitmask = field.inverted_mask<std::remove_volatile_t<T>>();

    // Clear width's number of bits in the target value at the bit position
    // specified.
    bitset_ &= kInvertedBitmask;

    // AND value with mask to remove any bits beyond the specified width.
    // Shift masked value into bit position and OR with target value.
    bitset_ |= (static_cast<NormalT>(value) << field.position) & kBitmask;

    return *this;
  }

  template <bitrange mask>
  [[nodiscard]] constexpr auto extract() const {
    // Create mask by shifting the set of 1s down so that the number of 1s
    // from bit position 0 is equal to the width parameter.
    return std::bitset<mask.width>(bitset_.to_ullong() >> mask.position);
  }

 private:
  std::bitset<sizeof(T) * BITS_PER_BYTE> bitset_;
};

template <typename T>
class bitmanip : public utils::bitset<T> {
 public:
  explicit bitmanip(T& register_reference)
      : register_reference_(register_reference), temp_(register_reference) {}

  bitmanip& operator=(const bitmanip& other) = delete;
  bitmanip& operator=(const bitmanip&& other) = delete;
  bitmanip(const bitmanip& other) = delete;
  bitmanip(const bitmanip&& other) = delete;

  ~bitmanip() { save(); }

  auto& save() {
    if constexpr (!std::is_const_v<T>) {
      register_reference_ = this->to_ullong();
    }
    return *this;
  }

  auto& update() {
    register_reference_ = *this;
    return *this;
  }

 private:
  T& register_reference_;
};

}  // namespace utils