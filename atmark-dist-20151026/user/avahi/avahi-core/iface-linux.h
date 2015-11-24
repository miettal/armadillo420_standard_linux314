#ifndef fooifacelinuxhfoo
#define fooifacelinuxhfoo

/* $Id: iface-linux.h 726 2005-10-11 19:52:38Z lennart $ */

/***
  This file is part of avahi.
 
  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.
 
  avahi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
  Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

typedef struct AvahiInterfaceMonitorOSDep AvahiInterfaceMonitorOSDep;

#include "netlink.h"

struct AvahiInterfaceMonitorOSDep {
    AvahiNetlink *netlink;
        
    unsigned query_addr_seq, query_link_seq;
    
    enum {
        LIST_IFACE,
        LIST_ADDR,
        LIST_DONE
    } list;
};


#endif