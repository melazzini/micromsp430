/*
 * uarta.hpp
 *
 *  Created on: 9 θών. 2022 γ.
 *      Author: francisco
 */

#ifndef INCLUDE_UARTA_HPP_
#define INCLUDE_UARTA_HPP_

#include "utils.hpp"
#include<functional>

namespace uarta
{

enum class Clk_UartA : uint32_t
{
    UC0CLK = UCSSEL_0, ACLK = UCSSEL_1, SMCLK = UCSSEL_2
};

template<uint32_t clkFreq, uint32_t baudrate, typename std::enable_if<
        (clkFreq / baudrate) >= 3, bool>::type baudrate_ok = true>
struct UARTA_Config
{
    static constexpr uint32_t N_FACTOR = clkFreq / baudrate;

    uint16_t modulation;
    Clk_UartA clk;
    uint8_t bits; //7 or 8
    uint8_t order; //LSB or MSB
    uint8_t it_mode; // on off
    uint8_t ovesampling;
};

struct UART_A
{
    template<typename T_config>
    static constexpr void init(const T_config &config)
    {
        disableUSCI();
        setClk(config.clk);
        setAsyncMode();
        setBaudrateNFactor<T_config::N_FACTOR>();
        enableUSCI();
    }

    static constexpr void write(const uint8_t *data_, uint8_t size_)
    {
        for (uint8_t i = 0; i < size_; i++)
        {
            while (!(IFG2 & UCA0TXIFG))
                ;
            UCA0TXBUF = data_[i];
        }
    }

    static constexpr void write_it(const uint8_t *data_, uint8_t size_)
    {
        _txData = data_;
        _txSize = size_;
        _txCounter = 0;
        enableTxInterrupt();
    }

    static constexpr void read(uint8_t *data_, uint8_t size_,
                               uint8_t delimiter = '\n')
    {
        if (0 == size_)
        {
            return;
        }

        uint8_t i = 0;

        for (i = 0; i < size_ - 1; i++)
        {
            while (!(IFG2 & UCA0RXIFG))
                ;
            data_[i] = UCA0RXBUF;

            if (data_[i] == delimiter)
            {
                i++;
                break;
            }
        }

        data_[i] = '\0';
    }

    static constexpr void read_it(uint8_t *data_, uint8_t size_,
                                  void (*callback_)(uint8_t*, uint8_t),
                                  uint8_t delimiter_ = '\n')
    {
        _rxData = data_;
        _rxSize = size_;
        _rxCounter = 0;
        _rxCallback = callback_;
        _rxDelimiter = delimiter_;
        //_rxReadReady; //we don't touch this flag until: we have a result or an error
        enableRxInterrupt();
    }

    static constexpr void read_it(uint8_t *data_, uint8_t size_, int *ready,
                                  uint8_t delimiter_ = '\n')
    {
        _rxData = data_;
        _rxSize = size_;
        _rxCounter = 0;
//            _rxCallback = callback_;
        _rxDelimiter = delimiter_;
        _rxReadReady = ready;
        enableRxInterrupt();
    }

    static constexpr void performRead_it()
    {

        while (!(IFG2 & UCA0RXIFG))
            ;
        _rxData[_rxCounter] = UCA0RXBUF;

        if ((_rxData[_rxCounter] == _rxDelimiter)
                || (_rxCounter + 1 >= _rxSize - 1))
        {
            _rxData[++_rxCounter] = '\0';

            if (_rxReadReady)
            {
                *_rxReadReady = 1;
            }

            if (_rxCallback)
            {
                _rxCallback(_rxData, _rxCounter - 1);
            }

            _rxData = nullptr;
            _rxSize = 0;
            _rxCounter = 0;
            _rxCallback = nullptr;
            _rxDelimiter = 0;
            _rxReadReady = nullptr;
            disableRxInterrupt();
        }
        else
        {
            _rxCounter++;
        }

    }

    /**
     * Call this function inside the tx isr
     * in order to transfer the data that you
     * passed in write_it()
     */
    static constexpr void performWrite_it()
    {
        if ((_txCounter >= _txSize) || (_txData == nullptr))
        {
            _txCounter = 0;
            _txSize = 0;
            _txData = nullptr;
            disableTxInterrupt();
        }
        else
        {
            UCA0TXBUF = _txData[_txCounter++];
        }
    }

    static constexpr void enableTxInterrupt()
    {
        IE2 |= UCA0TXIE;
    }

    static constexpr void disableTxInterrupt()
    {
        IE2 &= ~UCA0TXIE;
    }

    static constexpr void enableRxInterrupt()
    {
        IE2 |= UCA0RXIE;
    }

    static constexpr void disableRxInterrupt()
    {
        IE2 &= ~UCA0RXIE;
    }

private:
    static const uint8_t *_txData;
    static uint8_t _txSize;
    static uint8_t _txCounter;

    static void (*_rxCallback)(uint8_t*, uint8_t);
    static int *_rxReadReady; // +-ready, --error/undefined/idle, 0-not ready
    static uint8_t *_rxData;
    static uint8_t _rxSize;
    static uint8_t _rxCounter;
    static uint8_t _rxDelimiter;

private:
    static constexpr void disableUSCI()
    {
        UCA0CTL1 |= UCSWRST;
    }

    static constexpr void enableUSCI()
    {
        UCA0CTL1 &= ~UCSWRST;
    }

    static constexpr void setClk(Clk_UartA clk)
    {
        clearBits(UCA0CTL1, 6, 2);
        UCA0CTL1 |= static_cast<uint32_t>(clk);
    }

    static constexpr void setAsyncMode()
    {
        UCA0ABCTL &= ~UCSYNC;
    }

    template<uint32_t N_factor>
    static constexpr void setBaudrateNFactor()
    {
        UCA0BR0 |= static_cast<uint8_t>(N_factor & 0x00FF);
        UCA0BR1 |= static_cast<uint8_t>((N_factor & 0xFF00) >> 8);
    }
};

}

#endif /* INCLUDE_UARTA_HPP_ */
