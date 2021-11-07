#pragma once

#include <cstdint>
#include <variant>

#include "Common.h"
#include "Register.h"

namespace Gpio {
enum PinBit : std::uint16_t {
  kPin0 = 1,
  kPin1 = 1 << 1,
  kPin2 = 1 << 2,
  kPin3 = 1 << 3,
  kPin4 = 1 << 4,
  kPin5 = 1 << 5,
  kPin6 = 1 << 6,
  kPin7 = 1 << 7,
  kPin8 = 1 << 8,
  kPin9 = 1 << 9,
  kPin10 = 1 << 10,
  kPin11 = 1 << 11,
  kPin12 = 1 << 12,
  kPin13 = 1 << 13,
  kPin14 = 1 << 14,
  kPin15 = 1 << 15
};

using AlternateFunction = std::uint8_t;

enum Speed : std::uint8_t {
  kLow = 0b00,
  kMedium = 0b01,
  kHigh = 0b10,
  kVeryHigh = 0b11
};

/**
 * 00: Input mode (reset state)
 * 01: General purpose output mode
 * 10: Alternate function mode
 * 11: Analog mode
 */
enum Mode : std::uint8_t {
  kInput = 0b00,
  kOutput = 0b01,
  kAlternate = 0b10,
  kAnalog = 0b11
};

/**
 * 00: No pull-up, pull-down
 * 01: Pull-up
 * 10: Pull-down
 */
enum PullUpDown : std::uint8_t {
  kNone = 0b00,
  kPullUp = 0b01,
  kPullDown = 0b10
};

enum Type : std::uint8_t { kOutputPushPull = 0, kOutputOpenDrain = 1 };

struct Config {
  Mode mode;
  Type type = Type::kOutputPushPull;
  Speed speed = Speed::kLow;
  PullUpDown pull_up_down = PullUpDown::kNone;
  AlternateFunction af = 0x00;
};

class Port {
 public:
  void ConfigureMode(std::uint16_t pin_num, Mode mode) {
    mode_.ApplyMask(0b11, pin_num, mode);
  }

  void ConfigureType(std::uint16_t pin_num, Type type) {
    type_.ApplyMask(0b1, pin_num, type);
  }

  void ConfigureSpeed(std::uint16_t pin_num, Speed speed) {
    speed_.ApplyMask(0b11, pin_num * 2, speed);
  }

  void ConfigurePullUpDown(std::uint16_t pin_num, PullUpDown pull_up_down) {
    pull_up_down_.ApplyMask(0b11, pin_num * 2, pull_up_down);
  }

  void ConfigureAlternateFunction(std::uint16_t pin_num, uint8_t af) {
    // TODO: check if this logic works, because I am using a 64-bit register
    // to represent two 32-bit registers. Does endianness play a role here?
    alternate_fun_.ApplyMask(0b1111, pin_num * 4, af);
  }

  bool IsPinChosen(std::uint16_t pins, std::uint16_t pin_num) {
    return (pins >> pin_num) & 0b1;
  }

  Status Configure(std::uint16_t pins, const Config& config) {
    // TODO: Check if register read-writes could be optimized by
    // transposing the steps, i.e. ConfigureMode would write
    // all pin values at once.
    // TODO: Better understand the cost of read-writing relative
    // to CPU cycles -- is it better to have more steps in RAM
    // to avoid interacting with the peripherals?
    for (uint16_t pin_num = 0; pin_num < 16; pin_num++) {
      if (IsPinChosen(pins, pin_num)) {
        ConfigureMode(pin_num, config.mode);

        // TODO: How would be the best switching structure
        // to perform these kinds of configurations?
        // -- std::variant + visit?
        // -- switch + case?
        // -- nested if/else?
        if (config.mode == Mode::kInput) {
          ConfigurePullUpDown(pin_num, config.pull_up_down);
        }

        if (config.mode == Mode::kOutput) {
          ConfigurePullUpDown(pin_num, config.pull_up_down);
          ConfigureSpeed(pin_num, config.speed);
          ConfigureType(pin_num, config.type);
        }

        if (config.mode == Mode::kAlternate) {
          ConfigurePullUpDown(pin_num, config.pull_up_down);
          ConfigureSpeed(pin_num, config.speed);
          ConfigureType(pin_num, config.type);
          ConfigureAlternateFunction(pin_num, config.af);
        }
      }
    }

    return Status::kOk;
  }

  bool ReadPin(PinBit bit) { return input_data_ & bit; }

  void SetPin(PinBit bit) { atomic_set_reset_ = bit; }

  void ResetPin(PinBit bit) { atomic_set_reset_ = (bit << 16); }

  void TogglePin(PinBit bit) { output_data_ ^= bit; }

  void LockPin() {
    // Not implemented
    DoNothing();
  }

 private:
  volatile Register<32, ReadWrite> mode_;           // MODER
  volatile Register<16, ReadWrite> type_;           // OTYPER
  volatile Register<16, Reserved> reserved_0_;      // Reserved
  volatile Register<32, ReadWrite> speed_;          // OSPEEDR
  volatile Register<32, ReadWrite> pull_up_down_;   // PUPDR
  volatile Register<16, Read> input_data_;          // IDR
  volatile Register<16, Reserved> reserved_1_;      // Reserved
  volatile Register<16, ReadWrite> output_data_;    // ODR
  volatile Register<16, Reserved> reserved_2_;      // Reserved
  volatile Register<32, Write> atomic_set_reset_;   // BSRR (MSB is reset)
  volatile Register<32, ReadWrite> lock_;           // LCKR
  volatile Register<64, ReadWrite> alternate_fun_;  // AFR
};
}  // namespace Gpio