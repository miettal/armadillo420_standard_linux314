#ifndef _HERMIT_TARGET_NET_SMSC911X_H_
#define _HERMIT_TARGET_NET_SMSC911X_H_

#include <net/net.h>

extern struct net_device smsc911x;

extern int smsc911x_get_mac(unsigned char *mac);

#endif /* _HERMIT_TARGET_NET_SMSC911X_H_ */
