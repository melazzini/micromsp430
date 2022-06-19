#ifndef GPIO_HPP_
#define GPIO_HPP_

#include "utils.hpp"
#include<type_traits>

namespace gpio
{

enum class Id
{
    _1, _2, _3
};

/**
 * @brief This represents the direction of the gpio port.
 *
 */
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

template<Id p>
struct _Port12Hnd
{
    static constexpr uint8_t _UNUNSED_BYTES = (Id::_1 == p) ? 25U : 19U;
    _io uint8_t m_input; //
    _io uint8_t m_output; //
    _io uint8_t m_direction; //
    _io uint8_t m_it_flag; //
    _io uint8_t m_it_edge_sel; //
    _io uint8_t m_it_ena; //
    _io uint8_t m_port_sel; //
    _io uint8_t res_ena; //
    uint8_t _unusedval00[_UNUNSED_BYTES]; //
    _io uint8_t m_port_sel2; //
};

/**
 * @brief This class represents a handler for P1 or P2 gpio port.
 *
 * @tparam id Id of the port
 * @tparam bit The pins of the port
 */
template<Id id, uint8_t bit>
class PORT12
{
private:
    static constexpr uint16_t _P1_ADDRESS = 0x20;
    static constexpr uint16_t _P2_ADDRESS = 0x28;

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

    inline constexpr void toggle() const
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

    typename std::enable_if<id == Id::_1 || id == Id::_2, _Port12Hnd<id>*>::type m_port =
            reinterpret_cast<_Port12Hnd<id>*>(
                    (id == Id::_1) ? (_P1_ADDRESS) : (_P2_ADDRESS));
};

}

#endif /* GPIO_HPP_ */
