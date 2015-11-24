#define CORE_NAME "netdev"

#include <herrno.h>
#include <htypes.h>
#include <string.h>
#include <net/net.h>
#include <net/arp.h>

inline void netdev_set_ipaddr(struct net_device *dev, unsigned char *ipaddr)
{
	memcpy(dev->eth_ipaddr, ipaddr, 4);
}

inline unsigned char *netdev_get_ipaddr(struct net_device *dev)
{
	return dev->eth_ipaddr;
}

int netdev_init(struct net_device *dev)
{
	int ret = 0;

	if (dev == NULL)
		return -H_EINVAL;
	if (dev->init)
		ret = dev->init(dev);

	arp_cache_clear();

	return ret;
}

int netdev_exit(struct net_device *dev)
{
	int ret = 0;

	if (dev == NULL)
		return -H_EINVAL;
	if (dev->exit)
		ret = dev->exit(dev);

	return ret;
}
