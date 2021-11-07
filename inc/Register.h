#pragma once

#include <cstdint>
#include <type_traits>

#include "RegisterTraits.h"

template <std::size_t Size, typename AccessType = ReadWrite>
struct Register {
 public:
  using RegisterType = typename RegisterTraits<Size>::type;
  using RegisterMask = RegisterType (*)(RegisterType);

  template <typename Trait>
  constexpr void AssertAccess() const volatile {
    if constexpr (!std::is_base_of<Trait, AccessType>()) {
      static_assert(DependentFalse<AccessType>());
    }
  }

  volatile void operator=(RegisterType bit_mask) volatile {
    AssertAccess<Write>();
    register_ = bit_mask;
  }

  operator RegisterType() const volatile {
    AssertAccess<Read>();
    return register_;
  }

  void operator|=(RegisterType bit_mask) volatile {
    AssertAccess<ReadWrite>();
    register_ |= bit_mask;
  }

  void operator&=(RegisterType bit_mask) volatile {
    AssertAccess<ReadWrite>();
    register_ &= bit_mask;
  }

  void operator^=(RegisterType bit_mask) volatile {
    AssertAccess<ReadWrite>();
    register_ ^= bit_mask;
  }

  void ApplyMask(std::uint8_t mask, std::uint8_t offset,
                 std::uint8_t value) volatile {
    AssertAccess<ReadWrite>();
    RegisterType temp = register_ & ~(mask << offset);
    temp |= (mask & value) << offset;
    register_ = temp;
  }

  volatile RegisterType register_;
};

template <std::size_t Size, typename AccessType = ReadWrite>
constexpr volatile Register<Size, AccessType>* MakeRegister(uint32_t address) {
  return reinterpret_cast<volatile Register<Size, AccessType>*>(address);
}