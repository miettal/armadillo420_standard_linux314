#ifndef _HERMIT_TARGET_PMIC_MC34704_H_
#define _HERMIT_TARGET_PMIC_MC34704_H_

#include <htypes.h>

extern int mc34704_read(u8 addr, u8 *buf);
extern int mc34704_write(u8 addr, u8 buf);

#endif
