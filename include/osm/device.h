#ifndef OSM_DEVICE_H
#define OSM_DEVICE_H

#include <osm/utils.h>

/*
 * Define connection types
 */
#define OSM_CT_FILE 0
#define OSM_CT_TCP 1

/**
 * Device context: can be used to interact with an underlying device
 */
typedef struct {
	char *name;
	unsigned int conn_type;
	char *address;
	Vector inputs, outputs;
} OSMDevice;

#endif
