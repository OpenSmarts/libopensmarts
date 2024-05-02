#include "osm/bind.h"
#include "osm/utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <threads.h>


#define MAX_ID 0xFFFF

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
	uint id = 0;
	uint offset = offsetof(struct sockaddr_un, sun_path);
	while(_need_chars(id) + len < sizeof(name.sun_path) - 1)
	{
		if (id > MAX_ID)
			return -1;

		// write file name
		_write_name(id, name.sun_path + len);
		
		// try to bind
		errno = 0;
		int err = bind(sockfd, (struct sockaddr *) &name, offset + strlen(name.sun_path));

		if (err == 0)
		{
			printf("Bound!\n");
			return 0;
		}
		else if (errno != EADDRINUSE)
		{
			// If not bound and it's not due to an address in use error
			// then we have an actual problem
			return -1;
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
	int sockfd = socket(AF_LOCAL, SOCK_SEQPACKET, 0);
	if (sockfd == -1)
	{
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
	
	if (bound != 0)
	{
		close(sockfd);
		return -1;
	}
	
	listen(sockfd, 3);
	return sockfd;
}

/**
 * Listen for and return connections for a socket
 * return - a vector containing all the threads which this function started
 */
Vector osm_listen_and_accept(int sockfd, thrd_start_t callback)
{
	Vector out = vect_init(sizeof(thrd_t));
	thrd_t thread;

	while(1)
	{
		// Try to accept a connection
		errno = 0;
		int fd = accept(sockfd, NULL, NULL);
		if (fd == -1)
		{
			if (errno != ECONNABORTED && errno != EINTR)
			{
				break;
			}
			else
				continue;
		}
		
		// Create and store the connection thread
		errno = 0;
		int ret = thrd_create(&thread, callback, (void*)(uintptr_t)fd);
		if(ret == thrd_success)
		{
			vect_push(&out, &thread);
		}
		else
		{
			if (ret == thrd_error)
			{
				perror("Error creating thread");
			}
			else if (ret == thrd_nomem)
			{
				fprintf(stderr, "Thread out of memory. Shutting down.\n");
			}
			break;
		}

	}

	return out;
}

/**
 * Bind a new network socket
 * Should only be called by one process on the machine.
 *
 * If more than one device needs to be exposed through this system, use a
 * master process to handle internet traffic and export each device as a
 * sub-device.
 *
 * return - a negative number on error, or the 
 */
int osm_open_network()
{
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	return sockfd;
}

