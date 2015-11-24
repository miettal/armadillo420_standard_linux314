
#ifndef _MX3_PCMCIA_H_
#define _MX3_PCMCIA_H_

/* POR: PCMCIA Option Register 0-4*/
#define POR_PV (BIT(29))

#define PGCR_RESET (BIT(0))
#define PGCR_POE (BIT(1))
#define PGCR_SPKREN (BIT(2))
#define PGCR_LPMEN (BIT(3))

#endif
