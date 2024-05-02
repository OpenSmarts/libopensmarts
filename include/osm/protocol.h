#ifndef OSM_PROTOCOL_H
#define OSM_PROTOCOL_H

#include <stdint.h>

// The types used in connections

/*
 * Before signatures can be validated, we need to get
 * a public verification key, along with the firendly
 * name and type of device.
 *
 * These are those initial connection frames.
 */

/// The magic number for init frames
extern const char OSM_MAGIC_INIT[4];
/// The magic number for normal frames
extern const char OSM_MAGIC_FRAME[4];

/**
 * The init frame from the controller
 * includes the uuid of the sender and the public
 * key the sender will use to sign messages (up to 4096 bytes)
 *
 * The paired device will send one back at the end of the pairing
 * process.  In the end frame, the pubkey is encrypted by the pairing code.
 */
typedef struct {
	uint8_t magic[4];
	uint8_t uuid[8];
	uint8_t keytype;
	uint16_t keylen;
	uint8_t pubkey[4096];
} OSMInitFrame;

/**
 * Represents the format of the pairing code type that the
 * initializing device will give the user.
 */
typedef struct {
	uint8_t magic[4];
	uint8_t uuid[8];

} OSMInitPairfmt;


/**
 * All OSM messages are at most 2048 bytes long and include a gpg signature at
 * the start of the data to validate the sender.  The signature length is determined based on
 * the signing key of the sender.
 */
typedef struct {
	uint8_t magic[4];          // magic number
	uint8_t uuid[8];           // device that this message is from
	uint8_t sub_uuid[8];       // sub device that this message is from
	uint8_t frame_type;        // type of frame
	uint8_t data[2048];       // message data
} OSMFrame;

/*
 * Once the data has been validated, it is one of the following frame types
 */

/// Result of previously sent frame, with information requested (if any)
#define OSM_FT_RES 0
/// Set a sensor/config value on the device
#define OSM_FT_SET 1
/// Get a sensor/config value from the device
#define OSM_FT_GET 2
/// Send data
#define OSM_FT_DAT 3
/// Setup new stream (value out to device FROM THIS MACHINE)
#define OSM_FT_SVO 4
/// Setup new stream (value in from device TO THIS MACHINE)
#define OSM_FT_SVI 5
/// Close stream
#define OSM_FT_SCL 6

/**
 * Result of a sent frame
 */
typedef struct {
	uint8_t res_type;   // the type of frame sent
	uint8_t a;
} OSMResFrame;

typedef struct {
	uint8_t num_set;
	uint8_t pairs[2047];
} OSMSetFrame;

typedef struct {
	uint8_t control_id[8];       // The ID of the control we are setting the value on
	uint8_t val_type;            // The data type of the value (or 0 for future data frames)
	uint8_t value[8];            // The new value for the control, or the data frame number we will next use
} OSMSetControl;



#endif

