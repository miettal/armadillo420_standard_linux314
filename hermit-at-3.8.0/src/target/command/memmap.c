#include <platform.h>
#include <herrno.h>
#include <map.h>
#include <io.h>
#include <flash.h>
#include <string.h>

#if defined(CONFIG_FLASH)
static int print_memmap_flash(void)
{
	struct platform_info *pinfo = &platform_info;
	struct region region;
	char *ptr = get_current_flash_map();
	addr_t map_base = pinfo->map->flash.base;
	size_t map_size = pinfo->map->flash.size;

	if (map_base == -1 || map_size == -1)
		return 0;

	ptr = strstr(ptr, ":");
	if (!ptr)
		return -H_EINVAL;
	ptr++;

	memzero(&region, sizeof(struct region));

	while (1) {
		addr_t start, end;
		char block[64];
		parse_region(ptr, &ptr, &region);
		start = map_base + region.offset;
		end = (region.size == ~0) ? map_base + map_size - 1 :
			map_base + region.offset + region.size - 1;
		flash_get_block_string(block, 64, start, end,
				       region.flag & R_RO);
		hprintf("%p:%p FLA %s %s\n",
			start, end, region.name, block);
		if (ptr[0] != ',')
			break;
		ptr++;
		region.offset = region.offset + region.size;
	}

	return 0;
}
#else
#define print_memmap_flash()
#endif

static int print_memmap_dram(void)
{
	struct platform_info *pinfo = &platform_info;
	struct region region;
	char *ptr = pinfo->default_ramparts;
	addr_t map_base = pinfo->map->ram.base;
	size_t map_size = pinfo->map->ram.size;

	if (map_base == -1 || map_size == -1)
		return 0;

	memzero(&region, sizeof(struct region));

	while (1) {
		addr_t start, end;
		parse_region(ptr, &ptr, &region);
		start = map_base + region.offset;
		end = (region.size == ~0) ? map_base + map_size - 1 :
			map_base + region.offset + region.size - 1;
		hprintf("%p:%p RAM %s\n",
			start, end, region.name);
		if (ptr[0] != ',')
			break;
		ptr++;
		region.offset = region.offset + region.size;
	}
	return 0;
}

static int memmap_cmdfunc(int argc, char *argv[])
{
	print_memmap_flash();
	print_memmap_dram();

	return 0;
}

const command_t memmap_command = {
	.name = "memmap",
	.arghelp = "",
	.cmdhelp = "output memory map",
	.func = &memmap_cmdfunc
};

COMMAND(memmap_command);
