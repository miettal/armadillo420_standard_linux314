/* Nessus Attack Scripting Language 
 *
 * Copyright (C) 2002 - 2003 Michel Arboi and Renaud Deraison
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2,
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * In addition, as a special exception, Renaud Deraison and Michel Arboi
 * give permission to link the code of this program with any
 * version of the OpenSSL library which is distributed under a
 * license identical to that listed in the included COPYING.OpenSSL
 * file, and distribute linked combinations including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to
 * do so, delete this exception statement from your version.
 *
 */
#include "includes.h"
#include "nasl_raw.h"

#include "nasl_tree.h"
#include "nasl_global_ctxt.h"
#include "nasl_func.h"
#include "nasl_var.h"
#include "nasl_lex_ctxt.h"
#include "exec.h"

#include "nasl_packet_forgery.h"
#include "nasl_debug.h"

#include "nasl_socket.h"
#include "nasl_http.h"
#include "nasl_host.h"
#include "nasl_text_utils.h"
#include "nasl_nessusd_glue.h"
#include "nasl_misc_funcs.h"
#include "nasl_crypto.h"




/* **************************************************************** */

typedef struct {
  const char	*name;
  tree_cell*	(*c_code)(lex_ctxt*);
  int		unnamed;	/* Nb of unnamed args */
  const char	*args[16];	/* Must be sorted! Finish list with NULL */
} init_func;


static init_func libfuncs[] = {
  "script_name",	script_name, 1,
  { "deutsch", "english", "francais", "portugues", NULL },
  "script_version",     script_version, 1, { NULL }, 
  "script_timeout",     script_timeout, 1, { NULL },
  "script_description", script_description, 999,
  { "deutsch", "english", "francais", "portugues", NULL },
  "script_copyright",   script_copyright, 999,
  { "deutsch", "english", "francais", "portugues", NULL },
  "script_summary",     script_summary, 999,
  { "deutsch", "english", "francais", "portugues", NULL },
  "script_category",    script_category, 1, { NULL },
  "script_family",	script_family, 999,
  { "deutsch", "english", "francais", "portugues", NULL },
  "script_dependencie", script_dependencie, 999, { NULL },
  "script_dependencies",script_dependencie, 999, { NULL },
  "script_require_keys",script_require_keys, 999, { NULL },
  "script_require_ports", script_require_ports, 999,  { NULL },
  "script_require_udp_ports", script_require_udp_ports, 999, { NULL },
  "script_exclude_keys", script_exclude_keys, 999, { NULL },
  "script_add_preference", script_add_preference, 0,  { "name", "type", "value", NULL },
  "script_get_preference", script_get_preference, 1, { NULL }, 
  "script_id", 		script_id, 1, { NULL },
  "script_cve_id",      script_cve_id, 999, { NULL },
  "script_bugtraq_id",  script_bugtraq_id, 999, { NULL },
  "script_xref",	script_xref, 0, {"name", "value", NULL}, 
  "safe_checks", 	safe_checks, 	  0, { NULL },
  "set_kb_item",        set_kb_item, 	    0, {"name", "value", NULL },
  "get_kb_item",        get_kb_item,	    1, { NULL },
  "get_kb_list", 	get_kb_list,	    1, { NULL },
  "security_warning",   security_warning,   1, { "data", "port", "proto", "protocol", NULL },
  "security_note",      security_note,      1, { "data", "port", "proto", "protocol", NULL },
  "security_hole", 	security_hole,	    1, { "data", "port", "proto", "protocol", NULL },
 
  "open_sock_tcp",      nasl_open_sock_tcp,  1, { "timeout", "transport", NULL },
  "open_sock_udp",      nasl_open_sock_udp,  1, { NULL },
  "open_priv_sock_tcp", nasl_open_priv_sock_tcp, 0, { "dport", "sport", "timeout", NULL },
  "open_priv_sock_udp", nasl_open_priv_sock_udp, 0, { "dport", "sport", NULL },
  
  "recv",		nasl_recv, 0, { "length", "min", "socket", "timeout", NULL },
  "recv_line",		nasl_recv_line, 0, { "length", "socket", "timeout", NULL },
  "send", nasl_send, 0, { "data", "length", "option", "socket", NULL },
  "close",		nasl_close_socket, 1, {  NULL },
  "join_multicast_group", nasl_join_multicast_group, 1, { NULL },
  "leave_multicast_group", nasl_leave_multicast_group, 1, { NULL },
  
  
  "cgibin",		cgibin,		  0, { NULL },
  "is_cgi_installed",   nasl_is_cgi_installed, 1, {"item", "port", NULL },
  "http_open_socket",   http_open_socket, 1, { NULL },
  "http_head",		http_head,	  0, {"data", "item", "port", NULL },
  "http_get",   	http_get,	  0, {"data", "item", "port", NULL },
  "http_post",		http_post,	  0, {"data", "item", "port", NULL },
  "http_delete", 	http_delete,	  0, {"data", "item", "port", NULL },
  "http_put", 		http_put,	  0, {"data", "item", "port", NULL },
  "http_close_socket",	http_close_socket, 0, { "socket", NULL },
  "http_recv_headers", http_recv_headers, 1, { NULL },
  
  "get_host_name",	get_hostname, 0, { NULL }, 
  "get_host_ip",        get_host_ip,  0, { NULL },
  "get_host_open_port", get_host_open_port, 0, { NULL },
  "get_port_state",     get_port_state, 1, { NULL },
  "get_tcp_port_state", get_port_state, 1, { NULL },
  "get_udp_port_state", get_udp_port_state, 1, { NULL },
  "scanner_add_port",   nasl_scanner_add_port, 0, {"port", "proto", NULL },
  "scanner_status",     nasl_scanner_status, 0, {"current", "total", NULL },
  "scanner_get_port",   nasl_scanner_get_port, 1, { NULL },
  "islocalhost", 	nasl_islocalhost, 0, { NULL },
  "islocalnet",		nasl_islocalnet,  0, { NULL },
  "get_port_transport", get_port_transport, 1, { NULL },
  "this_host",  nasl_this_host, 0, { NULL }, 
  "this_host_name", nasl_this_host_name, 0, { NULL },
  
  
  "string", nasl_string, 9999, { NULL }, 
  "raw_string", nasl_rawstring, 9999, { NULL }, 
  "strcat", nasl_strcat, 9999, { NULL },

  "display", nasl_display, 9999, { NULL },
  "ord", nasl_ord, 1, { NULL },
  "hex",       nasl_hex, 1, { NULL },
  "hexstr",    nasl_hexstr, 1, { NULL },
  "strstr",	nasl_strstr, 2, { NULL },
  "ereg",	nasl_ereg, 0, { "icase", "pattern", "string", NULL },
  "ereg_replace", nasl_ereg_replace, 0, {"icase",  "pattern", "replace", "string", NULL },
  "egrep",	nasl_egrep, 0, { "icase", "pattern", "string", NULL },
  "eregmatch",	nasl_eregmatch, 0, { "icase", "pattern", "string", NULL },

  "match",	nasl_match, 0, { "icase", "pattern", "string", NULL },
  "substr",	nasl_substr, 3, { NULL },
  "insstr",	nasl_insstr, 4, { NULL },
  "tolower", 	nasl_tolower, 1, { NULL },
  "toupper",	nasl_toupper, 1, { NULL },
  "crap",	nasl_crap, 1, { "data", "length", NULL },
  "strlen", 	nasl_strlen, 1, { NULL },
  "split",	nasl_split, 1, { "keep", "sep", NULL },
  "chomp",	nasl_chomp, 1, { NULL },
  "int",	nasl_int,   1, { NULL },
  "stridx",	nasl_stridx, 3, { NULL },
  "str_replace", nasl_str_replace, 0, { "count", "find", "replace", "string", NULL },

  "make_list",	nasl_make_list, 9999, { NULL },
  "make_array",	nasl_make_array, 9999, { NULL },
  "keys",	nasl_keys, 9999, { NULL },
  "max_index",  nasl_max_index, 1, { NULL },
  "sort",	nasl_sort_array, 9999, { NULL },
   
  "telnet_init",	nasl_telnet_init, 1, { NULL },
  "ftp_log_in",	nasl_ftp_log_in, 0, {"pass", "socket", "user", NULL },
  "ftp_get_pasv_port", nasl_ftp_get_pasv_address, 0, { "socket", NULL }, 
  "start_denial", 	nasl_start_denial, 	  0, { NULL },
  "end_denial",	 	nasl_end_denial, 	  0, { NULL },

  "dump_ctxt", nasl_dump_ctxt, 0, { NULL },
  "typeof", nasl_typeof, 1, { NULL },

  "exit", nasl_do_exit, 1, { NULL },
  "rand", 	nasl_rand,    0, { NULL },
  "usleep", 	nasl_usleep,  1, { NULL },
  "sleep", 	nasl_sleep,  1, { NULL },
  "isnull",	nasl_isnull, 1, { NULL },
  "defined_func",	nasl_defined_func, 1, { NULL },

  "forge_ip_packet", forge_ip_packet, 0, 
  { "data", "ip_dst", "ip_hl", "ip_id", "ip_len", "ip_off", "ip_p", 
    "ip_src", "ip_sum", "ip_tos", "ip_ttl", "ip_v", NULL },
    
   "get_ip_element", get_ip_element, 0, { "element", "ip", NULL },
   "set_ip_elements", set_ip_elements, 0, 
   { "ip", "ip_dst", "ip_hl", "ip_id", 
     "ip_len", "ip_off", "ip_p", "ip_src", 
     "ip_sum", "ip_tos", "ip_ttl", "ip_v", NULL },
     
   "insert_ip_options", insert_ip_options, 0, { "code", "ip", "length", "value", NULL},
   "dump_ip_packet", dump_ip_packet, 9999, { NULL },
    
  "forge_tcp_packet", forge_tcp_packet, 0,
  { "data", "ip", "th_ack", "th_dport", "th_flags", "th_off", "th_seq",
    "th_sport", "th_sum", "th_urp", "th_win", "th_x2", "update_ip_len", NULL },
    
  "get_tcp_element", get_tcp_element, 0,
  { "element", "tcp", NULL },

  "set_tcp_elements", set_tcp_elements, 0, 
  { "data", "tcp", "th_ack", "th_dport", "th_flags", "th_off", "th_seq",
    "th_sport", "th_sum", "th_urp", "th_win", "th_x2", NULL },
    
   "dump_tcp_packet", dump_tcp_packet, 999, { NULL },
   "tcp_ping", 		nasl_tcp_ping,	    0, { "port", NULL },

  "forge_udp_packet", forge_udp_packet, 0,
  { "data", "ip", "uh_dport", "uh_sport", "uh_sum", "uh_ulen", "update_ip_len", NULL },
  
  "get_udp_element", get_udp_element, 0, 
  { "element", "udp", NULL },
  
  "set_udp_elements", set_udp_elements, 0,
  { "data", "udp", "uh_dport", "uh_sport", "uh_sum", "uh_ulen", NULL },
  
  "dump_udp_packet", dump_udp_packet, 999, { NULL },
  
  "forge_icmp_packet",forge_icmp_packet, 0,
  { "data", "icmp_cksum", "icmp_code", "icmp_id", "icmp_seq", "icmp_type", 
    "ip", "update_ip_len", NULL },
    
  "get_icmp_element", get_icmp_element, 0, 
  { "element", "icmp", NULL },

  "forge_igmp_packet", forge_igmp_packet, 0, 
  { "code", "data", "group", "ip", "type", "update_ip_len", NULL },
  "send_packet", nasl_send_packet, 99, 
  { "length", "pcap_active", "pcap_filter", "pcap_timeout", NULL },
  
  "pcap_next", nasl_pcap_next, 1, { "interface", "pcap_filter", "timeout"},
  
#ifdef HAVE_SSL
#ifdef HAVE_OPENSSL_MD2_H
  "MD2", nasl_md2, 1, { NULL },
#endif  
#ifdef HAVE_OPENSSL_MD4_H
  "MD4", nasl_md4, 1, { NULL },
#endif
  "MD5", nasl_md5, 1, { NULL }, 
  "SHA", nasl_sha, 1, { NULL },
  "SHA1", nasl_sha1, 1, { NULL },
  "RIPEMD160", nasl_ripemd160, 1, { NULL },
#ifdef HAVE_OPENSSL_MD2_H  
  "HMAC_MD2", nasl_hmac_md2, 0, { "data", "key", NULL },
#endif  
  "HMAC_MD5", nasl_hmac_md5, 0, { "data", "key", NULL },
  "HMAC_SHA", nasl_hmac_sha, 0, { "data", "key", NULL },
  "HMAC_SHA1", nasl_hmac_sha1, 0, { "data", "key", NULL },
  "HMAC_DSS", nasl_hmac_dss, 0, { "data", "key", NULL },
 
  "HMAC_RIPEMD160", nasl_hmac_ripemd160, 0, { "data", "key", NULL },
#endif  
  "NTLMv1_HASH",  nasl_ntlmv1_hash, 0, {"cryptkey", "passhash", NULL },
  "NTLMv2_HASH",  nasl_ntlmv2_hash, 0, {"cryptkey", "length", "passhash", NULL }, 
#ifdef HAVE_OPENSSL_MD4_H
  "nt_owf_gen", nasl_nt_owf_gen, 1, { NULL },
#endif
  "lm_owf_gen", nasl_lm_owf_gen, 1, { NULL },
  "ntv2_owf_gen", nasl_ntv2_owf_gen, 0, {"domain", "login", "owf", NULL },
  NULL, NULL, 0, { NULL }
} ;

static struct {
  const char	*name;
  int		val;
} libivars[] = {
  "TRUE",	1,
  "FALSE",	0,
  "pcap_timeout",	5,
  
  "IPPROTO_TCP", IPPROTO_TCP,
  "IPPROTO_UDP", IPPROTO_UDP,
  "IPPROTO_ICMP", IPPROTO_ICMP,
  "IPROTO_IP", IPPROTO_IP,
  "IPPROTO_IGMP", IPPROTO_IGMP,

  "ENCAPS_IP",     NESSUS_ENCAPS_IP,
  "ENCAPS_SSLv23", NESSUS_ENCAPS_SSLv23,
  "ENCAPS_SSLv2",  NESSUS_ENCAPS_SSLv2,
  "ENCAPS_SSLv3",  NESSUS_ENCAPS_SSLv3,
  "ENCAPS_TLSv1",  NESSUS_ENCAPS_TLSv1,

  "TH_FIN", TH_FIN,
  "TH_SYN", TH_SYN,
  "TH_RST", TH_RST,
  "TH_PUSH", TH_PUSH,
  "TH_ACK", TH_ACK,
  "TH_URG", TH_URG,

  "IP_RF", IP_RF,
  "IP_DF", IP_DF,
  "IP_MF", IP_MF,
  "IP_OFFMASK", IP_OFFMASK,

  "ACT_INIT", ACT_INIT, 
  "ACT_GATHER_INFO", ACT_GATHER_INFO,
  "ACT_ATTACK", ACT_ATTACK,
  "ACT_MIXED_ATTACK", ACT_MIXED_ATTACK,
  "ACT_DESTRUCTIVE_ATTACK", ACT_DESTRUCTIVE_ATTACK,
  "ACT_DENIAL", ACT_DENIAL,
  "ACT_SCANNER", ACT_SCANNER,
  "ACT_SETTINGS", ACT_SETTINGS,
  "ACT_KILL_HOST", ACT_KILL_HOST,
  "ACT_END", ACT_END,

  "MSG_OOB", MSG_OOB,

  NULL, 0, 
};

int init_nasl_library(lex_ctxt* lexic)
{
  int		i, j, c;
  nasl_func	*pf;
  named_nasl_var	*v;
  tree_cell	tc;
  const char	**p, *q;

  memset(&tc, 0, sizeof(tc));
  for (i = 0, c= 0; i < sizeof(libfuncs) / sizeof(libfuncs[0])-1; i ++)
    {
      if ((pf = insert_nasl_func(lexic, libfuncs[i].name, NULL)) == NULL)
	{
	  nasl_perror(lexic, "init_nasl2_library: could not define fct '%s'\n",
		  libfuncs[i].name);
	  continue;
	}
      pf->block = libfuncs[i].c_code;
      pf->flags |= FUNC_FLAG_INTERNAL;
      pf->nb_unnamed_args = libfuncs[i].unnamed;

      for (j = 0, p = libfuncs[i].args, q = NULL; (*p) != NULL; j ++)
	{
	  if (q != NULL && strcmp(q, *p) > 0)
	    nasl_perror(lexic, "init_nasl2_library: unsorted args for function %s: %s > %s\n", libfuncs[i].name, q, (*p));
	  q = (*p);
	  p ++;
	}
      pf->nb_named_args = j;
      pf->args_names = (char**)libfuncs[i].args;

      c++;
    }

  tc.type = CONST_INT;
  for (i = 0; i < sizeof(libivars) / sizeof(libivars[0])-1; i ++)
    {
      tc.x.i_val = libivars[i].val;
      if ((v = add_named_var_to_ctxt(lexic, libivars[i].name, &tc)) == NULL)
	{
	  nasl_perror(lexic, "init_nasl2_library: could not define var '%s'\n",
		  libivars[i].name);
	  continue;
	}
      c ++;
    }

  if (add_named_var_to_ctxt(lexic, "NULL", NULL) == NULL)
    nasl_perror(lexic, "init_nasl2_library: could not define var 'NULL'\n");

  return c;
}



char * nasl_version()
{
  static char vers[sizeof(VERSION) + 1];
  strncpy(vers, VERSION, sizeof(vers) - 1);
  vers[sizeof(vers) - 1 ] = '\0';
  return vers;
}
