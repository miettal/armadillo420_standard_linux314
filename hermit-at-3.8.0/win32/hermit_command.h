#ifndef _HERMIT_COMMAND_H_
#define _HERMIT_COMMAND_H_

#include <host/common/target.h>
#include <host/common/memmap.h>

#define F_FORCE_LOCKED 0x1

void restore_interactive(target_context_t *tc);
int CommandFinish(target_context_t **tc);
int CommandMemmap(char *szCom,target_context_t **tc);

int CommandDownload_Region(char *szCom,target_context_t **tc,
						   char *szFile,char *szRegion,unsigned long flags);
int CommandDownload_Address(char *szCom,target_context_t **tc,
							char *szFile,unsigned long addr,unsigned long flags);
extern int CommandErase_Region(char *szCom, target_context_t **tc,
			       char *szRegion, unsigned long flags);
extern int CommandErase_Address(char *szCom, target_context_t **tc,
				unsigned long addr, unsigned long flags);
#endif
