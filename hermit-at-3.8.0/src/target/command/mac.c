#include <command.h>
#include <io.h>
#include <herrno.h>
#include <mac.h>
#include <net/eth_util.h>

static int mac_cmdfunc(int argc, char *argv[]){
  unsigned char mac[6];

  get_mac_address(mac);
  print_mac(mac);

  return 0;
}

const command_t mac_command =
	{ "mac", 0, "display mac address", &mac_cmdfunc };

COMMAND(mac_command);
