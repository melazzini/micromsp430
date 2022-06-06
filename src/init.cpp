#include<msp430g2553.h>

namespace {
/**
 * This initialization goes before main()
 */
struct __LowLevelInitialization {

	__LowLevelInitialization() {
		WDTCTL = WDTPW + WDTHOLD;
//		BCSCTL3 |= LFXT1S_2;                      // LFXT1 = VLO
//		BCSCTL2 |= SELM_3;
		_BIS_SR(GIE);
//		//Set clock to 1Mhz
//		DCOCTL = 0;
//		BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
//		DCOCTL = CALDCO_1MHZ;
	}

};

__LowLevelInitialization  __initializer;
}
