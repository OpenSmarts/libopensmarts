#ifndef OSM_DEVICE_H
#define OSM_DEVICE_H

#include <stdint.h>

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

// Device datapoint

/// Raw data (rarely used)
#define OSM_TYPE_RAW    0        ///< Raw data that a script or extrnal program could process

#define OSM_TYPE_BOOL   1        ///< Boolean type
#define OSM_TYPE_INT    2        ///< Integer type
#define OSM_TYPE_FLOAT  3        ///< Floating point type
#define OSM_TYPE_COLOR  4        ///< Color type
#define OSM_TYPE_SELECT 5        ///< Selection type
#define OSM_TYPE_TIME   6        ///< Time type (duration)
#define OSM_TYPE_DATE   7        ///< Date type (exact date/time)

#define OSM_DDF_INPUT   0b0001   ///< This datapoint can be read
#define OSM_DDF_OUTPUT  0b0010   ///< This datapoint can be written
#define OSM_DDF_RANGE   0b0100   ///< This datapoint can be interpreted as two values which create a range
#define OSM_DDF_RANGED  0b1000   ///< This datapoint must be between a certain range

/// The device datapoint 
typedef struct {
	uint64_t id;       /// Unique id of the datapoint
	uint8_t *name;     /// Name of the datapoint in UTF-8
	uint8_t type;      /// Datatype of datapoint
	uint8_t flags;     /// Datapoint flags
} OSMDatapoint;

/// Attempt to read a datapoint from a device
/// returns nonzero error code on failure
int osm_read_datapoint(OSMDevice *dev, OSMDatapoint *dat, void *in);

/// Attempt to write a datapoint to a device
/// returns nonzero error code on failure
int osm_write_datapoint(OSMDevice *dev, OSMDatapoint *dat, void *out);


#endif
