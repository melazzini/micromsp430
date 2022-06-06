#ifndef GPIO_HPP_
#define GPIO_HPP_

#include "utils.hpp"

namespace gpio
{

template<unsigned p>
struct _Port12Hnd
{
    _io uint8_t m_input; //
    _io uint8_t m_output; //
    _io uint8_t m_direction; //
    _io uint8_t m_it_flag; //
    _io uint8_t m_it_edge_sel; //
    _io uint8_t m_it_ena; //
    _io uint8_t m_port_sel; //
    _io uint8_t res_ena; //
    uint8_t _unusedval00[(1 == p) ? 25 : 19]; //
    _io uint8_t m_port_sel2; //
};

enum class Dir : uint16_t
{
    INPUT, OUTPUT
};

enum class PUPD : uint16_t
{
    DIS = 0, ENA = 1,
};

enum class ALTF : uint16_t
{
    _00 = 0, _10 = 2, _11 = 3
};

template<uint8_t id, uint8_t bit>
class PORT12
{
public:
    constexpr PORT12(Dir dir, PUPD pupd = PUPD::DIS, ALTF altf = ALTF::_00)
    {
        m_port->m_direction &= ~bit;
        if (dir == Dir::OUTPUT)
        {
            m_port->m_direction |= bit;
        }

        setEnaDisPupd(pupd);
        setAltf(altf);
    }

    constexpr void turnOn() const
    {
        m_port->m_output |= bit;
    }

    constexpr void turnOff() const
    {
        m_port->m_output &= ~bit;
    }

    constexpr void toggle() const
    {
        m_port->m_output ^= bit;
    }

    constexpr void setEnaDisPupd(PUPD pupd)
    {
        m_port->res_ena &= ~bit;
        if (pupd == PUPD::ENA)
        {
            m_port->res_ena |= bit;
        }
    }

    constexpr void setAltf(ALTF altf)
    {
        auto altf_ = static_cast<uint16_t>(altf);

        m_port->m_port_sel &= ~bit;
        m_port->m_port_sel2 &= ~bit;

        if (BIT1 & altf_)
        {
            m_port->m_port_sel |= bit;
        }

        if (BIT0 & altf_)
        {
            m_port->m_port_sel2 |= bit;
        }
    }

private:
    _Port12Hnd<id> *m_port = reinterpret_cast<_Port12Hnd<id>*>(
            (id == 0x01) ? (0x20) : (0x28));
};

}

#endif /* GPIO_HPP_ */
