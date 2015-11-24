#define CORE_NAME "eth_util"

#include <hermit.h>
#include <io.h>

void print_ip(unsigned char *ip){
	dev_info_r("%d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
}

void print_mac(unsigned char *mac){
	dev_info_r("%b:%b:%b:%b:%b:%b\n",
		   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int is_valid_mac(unsigned char *mac)
{
	int i;

	for (i=0; i<6; i++)
		if (mac[i])
			return 1;

	return 0;
}
