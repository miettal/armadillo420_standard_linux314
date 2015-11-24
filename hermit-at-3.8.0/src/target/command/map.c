#include <platform.h>
#include <hermit.h>
#include <command.h>
#include <io.h>
#include <map.h>

static struct platform_info *pinfo = &platform_info;

struct map_list {
	char *name;
	struct map *map;
};

static int map_cmdfunc(int args, char *argv[]) {
	struct map_list list[] = {
		{ "     flash", &pinfo->map->flash, },
		{ "     param", &pinfo->map->param, },
		{ "       ram", &pinfo->map->ram, },
		{ "boot_param", &pinfo->map->boot_param, },
		{ " mmu_table", &pinfo->map->mmu_table, },
		{ "    kernel", &pinfo->map->kernel, },
		{ "    initrd", &pinfo->map->initrd, },
		{ "    gunzip", &pinfo->map->gunzip, },
		{ "      free", &pinfo->map->free, },
	};
	int i;

	for (i=0; i<ARRAY_SIZE(list); i++)
		hprintf("%s: %p - %p %s\n",
			list[i].name,
			list[i].map->size ? list[i].map->base : 0,
			list[i].map->size ? list[i].map->base +
			list[i].map->size : 0,
			list[i].map->size ? "" : "(undefined)");
	return 0;
}

const command_t map_command = {
	.name = "map",
	.arghelp = NULL,
	.cmdhelp = "display internal memory map",
	.func = map_cmdfunc,
};

COMMAND(map_command);
