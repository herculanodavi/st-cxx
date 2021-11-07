#pragma once

#include "Register.h"
#include "stm32f469xx.h"

namespace STM32L4xx {
enum class GpioPort { kA = 0, kB, kC, kD, kE };

void EnableGpio(GpioPort port) {
  Register<32> rcc_enable{RCC_BASE};
  rcc_enable |= (1 << static_cast<uint8_t>(port));
}

};  // namespace STM32L4xx