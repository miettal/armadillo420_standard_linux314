#include <autoconf.h>
#include <hermit.h>
#include <herrno.h>
#include <map.h>
#include <param.h>
#include <string.h>
#include <ctype.h>
#include <platform.h>
#include <flash.h>
#include <io.h>

char *get_current_flash_map(void)
{
#if defined(CONFIG_MTDPARTS_AUTO)
	struct platform_info *pinfo = &platform_info;
	static char mtdparts[CMDLINE_MAXLEN];
	int ret = -1;
	memzero(mtdparts, CMDLINE_MAXLEN);
#if defined(CONFIG_CMD_SETPARAM)
	ret = get_param_value("mtdparts", mtdparts, CMDLINE_MAXLEN);
#endif
	if (ret == 0)
		return mtdparts;
	else
#endif
#if defined(CONFIG_MTDPARTS_FIXED)
		return CONFIG_FIXED_MTDPARTS_STRINGS;
#else
		return pinfo->default_mtdparts;
#endif
}

int parse_region(char *ptr, char **retptr, struct region *region)
{
	if (!region)
		return -H_EINVAL;

	region->flag = 0;
	if (*ptr == '-') {
		region->size = ~0;
		ptr++;
	} else {
		region->size = strtol(ptr, &ptr, 0);
	}

	if (*ptr == '@') {
		ptr++;
		region->offset = strtol(ptr, &ptr, 0);
	}

	if (*ptr == '(') {
		char *endp;
		ptr++;
		endp = strstr(ptr, ")");
		if (endp) {
			memcpy(region->name, ptr, endp - ptr);
			region->name[endp - ptr] = '\0';
			ptr = endp + 1;
		}
	}

	if (strncmp(ptr, "ro", 2) == 0) {
		region->flag |= R_RO;
		ptr += 2;
	}
	if (strncmp(ptr, "lk", 2) == 0) {
		region->flag |= R_LK;
		ptr += 2;
	}
	*retptr = ptr;
	return 0;
}

static int get_region_by_name(char *ptr, char *name, struct region *region)
{
	memzero(region, sizeof(struct region));

	while (1) {
		parse_region(ptr, &ptr, region);
		if (strcmp(region->name, name) == 0)
			return 0;
		if (ptr[0] != ',')
			break;
		ptr++;
		region->offset = region->offset + region->size;
	}
	return -1;
}

int get_memmap_region_by_name(char *name, struct region *region)
{
	struct platform_info *pinfo = &platform_info;
	return get_region_by_name(pinfo->default_ramparts, name, region);
}

int get_flash_region_by_name(char *name, struct region *region)
{
	char *ptr = get_current_flash_map();

	ptr = strstr(ptr, ":");
	if (!ptr)
		return -H_EINVAL;
	ptr++;

	return get_region_by_name(ptr, name, region);
}

size_t get_flash_region_size_by_name(char *name)
{
	struct platform_info *pinfo = &platform_info;
	struct region region;
	size_t map_size = pinfo->map->flash.size;
	get_flash_region_by_name(name, &region);
	return region.size == ~0 ? map_size - region.offset : region.size;
}

addr_t get_flash_region_start_by_name(char *name)
{
	struct platform_info *pinfo = &platform_info;
	struct region region;
	addr_t map_base = pinfo->map->flash.base;
	get_flash_region_by_name(name, &region);
	return map_base + region.offset;
}

int get_flash_region_count(void)
{
	struct region region;
	char *ptr = get_current_flash_map();
	int count = 0;

	ptr = strstr(ptr, ":");
	if (!ptr)
		return -H_EINVAL;
	ptr++;

	memzero(&region, sizeof(struct region));

	while (1) {
		parse_region(ptr, &ptr, &region);

		if (ptr[0] != ',')
			break;
		ptr++;
		count++;
	}

	return count;
}
