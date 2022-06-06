#ifndef CLK_HPP_
#define CLK_HPP_
#include "utils.hpp"
#include<msp430g2553.h>

namespace clk
{

struct OscDco
{
    enum class Calibration : uint16_t
    {
        none, _1MHz, _8MHz, _12MHz, _16MHz
    };

    static constexpr void setCalibration(Calibration cal)
    {
        if (cal != Calibration::none)
        {
            clearBits(BCSCTL1, 0, 4);

            switch (cal)
            {
            case Calibration::_1MHz:

                orSetReg(BCSCTL1, CALBC1_1MHZ);
                DCOCTL = CALDCO_1MHZ;
                break;
            case Calibration::_8MHz:

                orSetReg(BCSCTL1, CALBC1_8MHZ);
                DCOCTL = CALDCO_8MHZ;
                break;
            case Calibration::_12MHz:

                orSetReg(BCSCTL1, CALBC1_12MHZ);
                DCOCTL = CALDCO_12MHZ;
                break;
            case Calibration::_16MHz:

                orSetReg(BCSCTL1, CALBC1_16MHZ);
                DCOCTL = CALDCO_16MHZ;
                break;
            default:
                break;
            }
        }

    }

    static constexpr void turnOn()
    {
        __bic_SR_register(SCG0);
    }

    static constexpr void turnOff()
    {
        __bis_SR_register(SCG0);
    }
};

struct Mclk
{
    enum class Osc : uint16_t
    {
        dco = 0, vlo = 0x02, lfxt1 = 0x03
    };

    enum class Div : uint16_t
    {
        _1 = 0x00, _2 = 0x1, _4 = 0x02, _8 = 0x03,
    };

    static constexpr void setOscillator(Osc osc)
    {
        // clear error flag, which is set on PUC
        clearBits(IFG1, 1, 1);

        if (osc == Osc::vlo)
        {
            clearBits(BCSCTL3, 4, 2);
            orSetReg(BCSCTL3, LFXT1S_2);

        }
        else if (osc == Osc::lfxt1)
        {
            clearBits(BCSCTL3, 4, 2);
        }

        orSetReg(BCSCTL2, static_cast<uint16_t>(osc));
    }

    static constexpr void setDivInputClk(Div div)
    {
        clearBits(BCSCTL2, 4, 2);
        orSetReg(BCSCTL2, static_cast<uint16_t>(div) << 4);
    }
};

}

#endif /* CLK_HPP_ */
