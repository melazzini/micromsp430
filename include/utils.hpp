#ifndef UTILS_HPP_
#define UTILS_HPP_
#include<msp430g2553.h>
#include <stdint.h>

typedef uint16_t Port;
typedef uint16_t Pin;
typedef uint16_t Level;

#define _io volatile

template<typename T_reg>
constexpr void clearBits(T_reg &reg, uint8_t pos, uint8_t n)
{
    T_reg ones = (1 << n) - 1; // 2**n - 1
    reg &= ~(ones << pos);
}

template<typename T_reg>
constexpr void orSetReg(T_reg &reg, uint16_t val)
{
    reg |= val;
}

#endif /* UTILS_HPP_ */
