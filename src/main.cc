#include <cstdint>
#include <iostream>

#include "Register.h"
#include "bitset.h"

int main() {
    constexpr auto a = utils::bitrange::from<0, 4>();
    auto b = utils::bitset(0b01111);
    auto c = b.extract<a>();

    std::cout << "Bitset is " << c << "\n";
    return 0;
}