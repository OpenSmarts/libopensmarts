#include "osm/bind.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

/**
 * The required characters to represent an id as a file name
 * id - the id to get translated to a file name
 */
int _need_chars(int id)
{
	int i = 1;

	while (id > 15)
	{
		id /= 16;
		i++;
	}

	return i;
}

/**
 * Write the integer file name after the folder path.
 * id - the id to get translated to a file name
 * start - a pointer to just after the end of the folder path string
 */
void _write_name(int id, char *start)
{
	int i = _need_chars(id);
	while (i > 0)
	{
		// Get current character
		char c = '0';
		if (id % 16 < 10)
			c += id % 16;
		else
			c = 'a' + (id % 16) - 10;
		
		// Put character
		*(start + i - 1) = c;

		// divide id, sub from i
		id /= 16;
		i -= 1;
	}
}

/**
 * Bind to the next available onboard socket in the given directory
 * sock_dir - The directory containing osm sockets (or null for the default)
 */
int osm_bind_local(int sockfd, const char *sock_dir)
{
	struct sockaddr_un name;
	name.sun_family = AF_LOCAL;
	
	strncpy(name.sun_path, sock_dir, sizeof(name.sun_path));
	name.sun_path[sizeof(name.sun_path) - 1] = 0;
	
	// Check for slash at end of path
	int len = strlen(name.sun_path);

	if (name.sun_path[len - 1] != '/')
	{
		name.sun_path[len] = '/';
		len++;
	}

	// check that we won't overflow the buffer
	if (len > sizeof(name.sun_path) - 2)
	{
		return -1;
	}

	// sequentially try to bind
	int id = 0;
	while(_need_chars(id) + len < sizeof(name.sun_path) - 1)
	{
		// write file name
		_write_name(id, name.sun_path + len);
		
		// try to bind
		int size = offsetof(struct sockaddr_un, sun_path) + strlen(name.sun_path);

		if (bind(sockfd, (struct sockaddr *) &name, size) > 0)
		{
			return 0;
		}

		// inc id
		id++;
	}

	return -1;
}

/**
 * Bind a new onboard socket
 * sock_dir - The directory containing osm sockets (or null for the default)
 * return - a negitve number on error, or the socket fd on success
 */
int osm_open_onboard(char *sock_dir)
{
	int sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return sockfd;
	}

	int bound = -1;
	if (sock_dir == NULL)
	{
		bound = osm_bind_local(sockfd, "/run/osm/onboard/");
	}
	else
	{
		bound = osm_bind_local(sockfd, sock_dir);
	}
	
	if (bound < 0)
	{
		perror("bind");
		close(sockfd);
		return bound;
	}

	return sockfd;
}




