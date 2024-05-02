#include "osm/utils.h"

#include <stdio.h>
#include <dirent.h>
#include <stddef.h>
#include <errno.h>

Vector osm_discover_onboard(char *sock_dir)
{
	Vector out = vect_init(sizeof(char));

	DIR *d = opendir(sock_dir);
	if (d)
	{
		struct dirent *dir;
		while((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_SOCK)
			{
			}
		}
		errno = 0;
		closedir(d);
	}
	else
	{
		perror("opendir");
	}

	return out;
}


