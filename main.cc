#include <iostream>

#include "Gpio.h"
#include "Register.h"

int main() {
  Gpio::Port gpio{};
  int i = sizeof(gpio);

  std::cout << "Hello there!"
            << "\n";
  return 0;
}