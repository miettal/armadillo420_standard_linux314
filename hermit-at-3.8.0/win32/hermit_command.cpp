#include <windows.h>

#include <host/common/target.h>
#include <host/common/memmap.h>
#include <host/common/options.h>
#include <host/common/download.h>
#include <host/common/erase.h>
#include <host/common/crc.h>
#include <host/common/serial.h>

#include "hermit.h"
#include "hermit_command.h"

void restore_interactive(target_context_t *tc)
{
	char c = '!';

	if(strcmp(tc->medium, "serial"))
		target_set_medium(tc, "serial");
	xwrite(tc->portfd, &c, 1);
}

int CommandFinish(target_context_t **tc){
	restore_interactive(*tc);
	target_close(*tc);
	return TRUE;
}

int CommandMemmap(char *szCom,target_context_t **tc){
	int ret;
	/* open a connection to the target */
	*tc = target_open(szCom, NULL);
	if(*tc == NULL){
		MessageBox(hDlg_Main, "can't open connection to target", "hermit", 0);
		return FALSE;
	}

	//set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_NORMAL);
	ret = target_connect(*tc);
	if(ret != 0){
		MessageBox(NULL, "can't connect to target", "hermit", 0);
		target_close(*tc);
		return FALSE;
	}

	target_write_command(*tc, "memmap");
	mem_map_read(*tc);

	return TRUE;
}

int CommandDownload_Region(char *szCom,target_context_t **tc,
						   char *szFile,char *szRegion,unsigned long flags){
	int ret;
	mem_region_t *mr;
	
	init_crc32();
	
	if(flags & F_FORCE_LOCKED)
		opt_force_locked = 1;
	else
		opt_force_locked = 0;
	
	mr = slurp_file_or_die(szFile);
	
	ret = CommandMemmap(szCom,tc);
	if(ret == FALSE) return FALSE;
	
	set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_FLASH);
	ret = download_to_region(*tc, mr, szRegion);
	set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_NORMAL);
	if(ret == -1) return FALSE;

	return TRUE;
}

int CommandDownload_Address(char *szCom,target_context_t **tc,
							char *szFile,unsigned long addr,unsigned long flags){
	int ret;
	mem_region_t *mr;
	
	init_crc32();
	if(flags & F_FORCE_LOCKED)
		opt_force_locked = 1;
	
	mr = slurp_file_or_die(szFile);
	
	ret = CommandMemmap(szCom,tc);
	if(ret == FALSE) return FALSE;
	
	set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_FLASH);
	ret = download_to_addr(*tc, mr, addr);
	set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_NORMAL);
	if(ret == -1) return FALSE;
	
	return TRUE;
}

int CommandErase_Region(char *szCom, target_context_t **tc,
			char *szRegion, unsigned long flags)
{
	int ret;

	init_crc32();

	if (flags & F_FORCE_LOCKED)
		opt_force_locked = 1;
	else
		opt_force_locked = 0;

	ret = CommandMemmap(szCom, tc);
	if (ret == FALSE)
		return FALSE;

	set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_FLASH);
	ret = erase_region(*tc, szRegion);
	set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_NORMAL);
	if (ret == -1)
		return FALSE;

	return TRUE;
}

int CommandErase_Address(char *szCom, target_context_t **tc,
			 unsigned long addr, unsigned long flags)
{
	int ret;

	init_crc32();

	if (flags & F_FORCE_LOCKED)
		opt_force_locked = 1;
	else
		opt_force_locked = 0;

	ret = CommandMemmap(szCom, tc);
	if (ret == FALSE)
		return FALSE;

	set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_FLASH);
	ret = erase_addr(*tc, addr);
	set_serial_timeout(((target_context_t *)*tc)->portfd, TIMEOUT_NORMAL);
	if (ret == -1)
		return FALSE;

	return TRUE;
}
