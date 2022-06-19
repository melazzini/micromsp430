/*
 * uarta.cpp
 *
 *  Created on: 9 θών. 2022 γ.
 *      Author: francisco
 */

#include"include/uarta.hpp"

const uint8_t *uarta::UART_A::_txData = nullptr;
uint8_t uarta::UART_A::_txSize = 0;
uint8_t uarta::UART_A::_txCounter = 0;

void (*uarta::UART_A::_rxCallback)(uint8_t*, uint8_t)=nullptr;
int *uarta::UART_A::_rxReadReady = nullptr;
uint8_t *uarta::UART_A::_rxData = nullptr;
uint8_t uarta::UART_A::_rxSize = 0;
uint8_t uarta::UART_A::_rxCounter = 0;
uint8_t uarta::UART_A::_rxDelimiter = 0;

//std::function<void()> uarta::UART_A::_callback2{};
