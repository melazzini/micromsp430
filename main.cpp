#include "main.hpp"
#include<string.h>
#include <cstdio>

void ConfigureMCLK();

using led_type = gpio::PORT12<gpio::Id::_1, BIT0|BIT6>;
using uartTxPin_type = gpio::PORT12<gpio::Id::_1, BIT2|BIT1>;

using namespace uarta;

using timer = timera::TimerA<timera::Id::_0>;

led_type *greenLed;

using uartConf_t = UARTA_Config<8'000'000,9'600>;
//uint8_t message[100]; // = "this is a simple message!!!\n\r";

#define ADC10CLK_OSC    ADC10SSEL_0
#define ADC10CLK_ACLK    ADC10SSEL_1
#define ADC10CLK_MCLK   ADC10SSEL_2
#define ADC10CLK_SMCLK  ADC10SSEL_3

#define ADC10CLKDIV_1    ADC10DIV_0
#define ADC10CLKDIV_2    ADC10DIV_1
#define ADC10CLKDIV_3    ADC10DIV_2
#define ADC10CLKDIV_4    ADC10DIV_3
#define ADC10CLKDIV_5    ADC10DIV_4
#define ADC10CLKDIV_6    ADC10DIV_5
#define ADC10CLKDIV_7    ADC10DIV_6
#define ADC10CLKDIV_8    ADC10DIV_7

#define ADC10VREF_0    SREF_0
#define ADC10VREF_1    SREF_1
#define ADC10VREF_2    SREF_2
#define ADC10VREF_3    SREF_3
#define ADC10VREF_4    SREF_4
#define ADC10VREF_5    SREF_5
#define ADC10VREF_6    SREF_6
#define ADC10VREF_7    SREF_7

#define ADC10SAMPLESIG_0 SHS_0
#define ADC10SAMPLESIG_1 SHS_1
#define ADC10SAMPLESIG_2 SHS_2
#define ADC10SAMPLESIG_3 SHS_3

#define ADC10SAMPLETIME_4 ADC10SHT_0
#define ADC10SAMPLETIME_8 ADC10SHT_1
#define ADC10SAMPLETIME_16 ADC10SHT_2
#define ADC10SAMPLETIME_64 ADC10SHT_3

#define ADC10INPUT_CH0 INCH_0
#define ADC10INPUT_CH1 INCH_1
#define ADC10INPUT_CH2 INCH_2
#define ADC10INPUT_CH3 INCH_3
#define ADC10INPUT_CH4 INCH_4
#define ADC10INPUT_CH5 INCH_5
#define ADC10INPUT_CH6 INCH_6
#define ADC10INPUT_CH7 INCH_7
#define ADC10INPUT_CH8 INCH_8
#define ADC10INPUT_CH9 INCH_9
#define ADC10INPUT_CH10 INCH_10
#define ADC10INPUT_CH11 INCH_11
#define ADC10INPUT_CH12 INCH_12
#define ADC10INPUT_CH13 INCH_13
#define ADC10INPUT_CH14 INCH_14
#define ADC10INPUT_CH15 INCH_15

struct ADC10_Config
{
    uint16_t clk;
    uint16_t clkDiv;
    uint16_t vRef;
    uint16_t sampleSignal;
    uint16_t sampleTime;
};

struct ADC10
{
    static constexpr void init(const ADC10_Config &config)
    {
        disableAdc10();
        configClk(config.clk, config.clkDiv);
        setVRef(config.vRef);
        setSampleSignal(config.sampleSignal);
        setSampleTime(config.sampleTime);
    }

    static constexpr uint16_t singleSample(uint16_t inputChannel)
    {
        setInputChannel(inputChannel);
        performSingleSampleIntoMemory();
        return readMemory();
    }
//    ADC10CTL1 = INCH_10 + ADC10DIV_0;

//    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON;
private:
    static constexpr void configClk(uint16_t clk, uint16_t div)
    {
        clearBits(ADC10CTL1, 3, 5);
        orSetReg(ADC10CTL1, clk | div);
    }

    static constexpr void setVRef(uint16_t vRef)
    {
        clearBits(ADC10CTL0, 13, 3);
        orSetReg(ADC10CTL0, vRef);
    }

    static constexpr void setSampleSignal(uint16_t sampleSignal)
    {
        clearBits(ADC10CTL1, 10, 2);
        orSetReg(ADC10CTL1, sampleSignal);
    }

    static constexpr void setSampleTime(uint16_t sampleTime)
    {
        clearBits(ADC10CTL0, 11, 2);
        orSetReg(ADC10CTL0, sampleTime);
    }

    static constexpr void performSingleSampleIntoMemory()
    {
        turnOnAdc10Internals();
        enableAdc10();
        __delay_cycles(100);
        disableAdc10();
        turnOffAdc10Internals();
    }

    static constexpr void setInputChannel(uint16_t inputChannel)
    {
        clearBits(ADC10CTL1, 12, 4);
        orSetReg(ADC10CTL1, inputChannel);
    }

    static constexpr void disableAdc10()
    {
        ADC10CTL0 &= ~ENC;
    }
    static constexpr void enableAdc10()
    {
//        ADC10CTL0 |= ENC;
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    }

    static constexpr void turnOnAdc10Internals()
    {
        orSetReg(ADC10CTL0, REFON | ADC10ON);
        __delay_cycles(100);
    }

    static constexpr void turnOffAdc10Internals()
    {
        clearBits(ADC10CTL0, 4, 2);
    }

    static constexpr uint16_t readMemory()
    {
        return ADC10MEM;
    }
};

int main(void)
{
    ConfigureMCLK();

    uint8_t message[32]; // = "this is a simple message!!!\n\r";

    int a = 10, b = 3;

//    sprintf((char*) message, "%d plus %d is %d", a, b, a + b);

    uartConf_t config_uart;
    config_uart.clk = Clk_UartA::SMCLK;
    UART_A::init(config_uart);
    uartTxPin_type txpin(gpio::Dir::OUTPUT, gpio::PUPD::DIS, gpio::ALTF::_11);

    led_type port { gpio::Dir::OUTPUT };
    port.turnOff();
    timera::Config config;
    config.clk = timera::Clk::SMCLK;
    config.countMode = timera::CountMode::UP;
    config.outputMode = timera::OutputMode::OUTPUT;
    timer::init(config);

    ADC10_Config configAdc;
    configAdc.clk = ADC10CLK_SMCLK;
    configAdc.vRef = ADC10VREF_0;
    configAdc.clkDiv = ADC10CLKDIV_1;
    configAdc.sampleSignal = ADC10SAMPLESIG_0;
    configAdc.sampleTime = ADC10SAMPLETIME_64;

    ADC10::init(configAdc);

    greenLed = &port;

    timer::tick(5'000);

//    UART_A::read_it(message, 100, [](uint8_t *message_, uint8_t size_)
//    {
//        UART_A::write_it(message_, size_);
//        timer::tick(1'000);
//    });

//    int ready = 0;

//    UART_A::read_it(message, 100, &ready);

    while (true)
    {
        uint32_t result = ADC10::singleSample(ADC10INPUT_CH10);

        uint16_t result_v = ((result * 3500) >> 10);

        uint16_t temp = (result_v - 996) >> 2;

//        sprintf((char*) message, "the value is %d mV; %d C\n",
//                static_cast<int>(result_v), static_cast<int>(temp));
        sprintf((char*) message, "%d\n",static_cast<int>(temp));

        UART_A::write(message, strlen((const char*) message));

        __delay_cycles(500'000);

//        if (ready)
//        {
//            timer::tick(2'000);
//            ready = 0;
//            UART_A::write_it(message, strlen((const char*) message));
//        }
//        UART_A::read(message, 100);
//        UART_A::write(message, strlen((const char*) message));
    }
}

void ConfigureMCLK()
{
    clk::OscDco::setCalibration(clk::OscDco::Calibration::_8MHz);
    clk::Mclk::setOscillator(clk::Mclk::Osc::dco);
    clk::Mclk::setDivInputClk(clk::Mclk::Div::_8);
}

