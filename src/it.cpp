#include"include/uarta.hpp"
#include"include/gpio.hpp"

int counter = 0;
extern gpio::PORT12<gpio::Id::_1, BIT0|BIT6> *greenLed;

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0(void)
{
    counter++;
    if (counter > 500)
    {
        greenLed->toggle();
        counter=0;
    }
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void TX_A0(void)
{
    uarta::UART_A::performWrite_it();
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void RX_A0(void)
{
    uarta::UART_A::performRead_it();
}



