
#ifndef _MX3_CCM_H_
#define _MX3_CCM_H_

/* CCMR : Control Register */
#define CCMR_PRCS  (BIT(1) | BIT(2))
#define      PRCS_FPM  0x2
#define      PRCS_CKIH 0x4
#define CCMR_MPE   (BIT(3))
#define CCMR_MDS   (BIT(7))
#define CCMR_SPE   (BIT(8))
#define CCMR_UPE   (BIT(9))
#define CCMR_PERCS (BIT(24))

#endif
