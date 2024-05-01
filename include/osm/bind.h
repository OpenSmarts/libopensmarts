#ifndef OSM_BIND_H
#define OSM_BIND_H

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

#endif
