#ifndef OSM_BIND_H
#define OSM_BIND_H

#include <osm/utils.h>
#include <threads.h>

/**
 * Bind to the next available onboard socket in the given directory
 * sock_dir - The directory containing osm sockets (or null for the default)
 */
int osm_bind_local(int sockfd, const char *sock_dir);

/**
 * Bind a new onboard socket
 * sock_dir - The directory containing osm sockets (or null for the default)
 * return - a negitve number on error, or the socket fd on success
 */
int osm_open_onboard(char *sock_dir);

/**
 * Listen for socket connections and return the new
 * file descriptors to the callback function for processing
 * sockfd - the socket file descriptor
 * callback - the callback function which will be provided with the new file descriptor
 */
Vector osm_listen_and_accept(int sockfd, thrd_start_t callback);

#endif
