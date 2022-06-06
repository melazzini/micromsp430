#ifndef TIMERA_HPP_
#define TIMERA_HPP_

#include"utils.hpp"

namespace timera
{

enum class Id
{
    _0, _1
};

enum class Clk : uint8_t
{
    TACLK, ACLK, SMCLK, INCLK
};

enum class CountMode : uint8_t
{
    STOP, UP, CONTINUOS, UPDOWN
};

enum class OutputMode : uint8_t
{
    MIN_VAL = 0,
    OUTPUT = MIN_VAL,
    SET = 0x01,
    TOGGLE_RESET = 0x02,
    SET_RESET = 0x03,
    TOGGLE = 0x04,
    RESET = 0x05,
    TOGGLE_SET = 0x06,
    RESET_SET = 0x07,
    MAX_VAL = RESET_SET
};

struct Config
{
    Clk clk;
    CountMode countMode;
    uint16_t operationMode;
    OutputMode outputMode;
};

template<Id id>
class TimerA
{
    struct _TypeDef
    {
        _io
        uint16_t CTL; //
        _io
        uint16_t CCTL0; //
        _io
        uint16_t CCTL1; //
        _io
        uint16_t CCTL2; //
        uint16_t _unused[4]; //
        _io
        uint16_t TAR; //
        _io
        uint16_t CCR0; //
        _io
        uint16_t CCR1; //
        _io
        uint16_t CCR2; //

    };

    static constexpr uint16_t _TIMER_A0_ADDRESS = 0x0160;
    static constexpr uint16_t _TIMER_A1_ADDRESS = 0x0180;

public:
    static constexpr void init(const Config &config_)
    {
        config = config_;
        configClk(config.clk);
        configCountMode(config.countMode);

        if (config.outputMode <= OutputMode::MAX_VAL)
        {
            timer->TA0CCTL1 &= ~(7 << 5);
            timer->TA0CCTL1 |= static_cast<uint16_t>(config.outputMode) << 5;
        }
    }

    static constexpr void tick(uint16_t cycles, uint16_t cycles_ccr1 = 0,
                               uint16_t cycles_ccr2 = 0)
    {
        timer->CCR0 = cycles;

        if (config.outputMode == OutputMode::OUTPUT)
        {
            timer->CCTL0 = CCIE;
        }

        if (cycles_ccr1)
        {
            timer->CCR1 = cycles_ccr1;
        }

        if (cycles_ccr2)
        {
            timer->CCR2 = cycles_ccr2;
        }
    }

    static constexpr void setCCR1(uint16_t cycles)
    {
        configCountMode(CountMode::STOP);
        timer->CCR1 = cycles;
        configCountMode(config.countMode);
    }

private:
    static constexpr _TypeDef *timer = reinterpret_cast<_TypeDef*>(
            (id == Id::_0) ? _TIMER_A0_ADDRESS : _TIMER_A1_ADDRESS);

    static Config config;

    static constexpr void configClk(Clk clk)
    {
        switch (clk)
        {
        case Clk::TACLK:
            timer->CTL &= ~(3 << 8);
            timer->CTL |= TASSEL_0;
            break;
        case Clk::ACLK:
            timer->CTL &= ~(3 << 8);
            timer->CTL = TASSEL_1;
            break;
        case Clk::SMCLK:
            timer->CTL &= ~(3 << 8);
            timer->CTL = TASSEL_2;
            break;
        case Clk::INCLK:
            timer->CTL &= ~(3 << 8);
            timer->CTL = TASSEL_3;
            break;
        default:
            break;
        }
    }

    static constexpr void configCountMode(CountMode countMode)
    {
        switch (countMode)
        {
        case CountMode::STOP:
            timer->CTL &= ~(3 << 4);
            timer->CTL |= MC_0;
            break;
        case CountMode::UP:
            timer->CTL &= ~(3 << 4);
            timer->CTL |= MC_1;
            break;
        case CountMode::CONTINUOS:
            timer->CTL &= ~(3 << 4);
            timer->CTL |= MC_2;
            break;
        case CountMode::UPDOWN:
            timer->CTL &= ~(3 << 4);
            timer->CTL |= MC_3;
            break;
        }
    }

    static constexpr CountMode getCurrentCountMount()
    {

        // TODO

        return CountMode::STOP;
    }
};

template<Id id>
Config TimerA<id>::config { };

}

#endif /* TIMERA_HPP_ */
//
//

//

//

//

//

//

//

