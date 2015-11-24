#ifndef _HERMIT_TARGET_RMA1_WDT_H_
#define _HERMIT_TARGET_RMA1_WDT_H_

extern void (*callback_rebooted_by_watchdog)(void);
extern void rma1_wdt_init(void);

#endif
