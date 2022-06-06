#include<msp430g2553.h>

volatile bool done = false;

void myDelay(unsigned ms) {
	done = false;
	CCTL0 = CCIE;
	CCR0 = 12U * ms;
	TACTL = TASSEL_1 + MC_1;
	while (!done)
		;
}


