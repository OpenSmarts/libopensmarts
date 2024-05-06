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
 * The init frame header from the controller
 * includes the uuid of the sender and the public
 * key the sender will use to sign messages (up to 4096 bytes)
 *
 * The paired device will send one back at the end of the pairing
 * process.  In the end frame, the pubkey is encrypted by the pairing code.
 */
typedef struct {
	uint8_t magic[4];
	uint8_t version;
	uint8_t uuid[8];
	uint8_t keytype;
	uint16_t keylen;
} OSMInitFrameHeader;

/**
 * The max possible length of an init frame.
 * The actual frame length will vary based on the size of the
 * pubkey.
 */
typedef struct {
	OSMInitFrameHeader header;
	uint8_t key[4096];
} OSMInitFrame;

/**
 * Represents the format of the pairing code type that the
 * initializing device will give the user.
 */
typedef struct {
	uint8_t magic[4];
	uint8_t uuid[8];
} OSMPairHeader;

/**
 * Represents a frame which describes
 */
typedef struct {
	OSMPairHeader header;
	uint8_t data[1024];        // message data
} OSMPairFrame;


/**
 * Main OSM Frame header: All frames contain this plus a secondary header
 * with more information based on the frame's type
 */
typedef struct {
	uint8_t magic[4];          // magic number
	uint8_t uuid[8];           // device that this message is from
	uint8_t sub_uuid[8];       // sub device that this message is from
	uint8_t frame_type;        // type of frame
} OSMFrameHeader;

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
	uint8_t res_type;   // the type of frame we are replying to
} OSMResHeader;

/**
 * Header for frames where we are attempting to set
 * data on the other device
 */
typedef struct {
	uint8_t num_set;
} OSMSetHeader;

/**
 * Header for frames where we are attempting to get
 * data from the other device
 */
typedef struct {
	uint8_t num_get;
} OSMGetHeader;

/**
 * Header for frames where we are sending
 * an arbitrary data payload to the other device
 */
typedef struct {
	uint8_t number;      // Which "set" this data frame belongs to (if multiple data streams are being sent at once)
	uint16_t len;        // How many significant bytes are in this frame
} OSMDataHeader;

/**
 * Header for streams where we are going to send a
 * continuous stream of data to the other device
 */
typedef struct {
} OSMStreamOutHeader;

/**
 * Header for streams where we are asking for
 * a continuous data stream from the other device
 */
typedef struct {
} OSMStreamInHeader;

/**
 * Header for a control frame where we are closing
 * a previously opened stream.
 */
typedef struct {
} OSMStreamCloseHeader;

typedef struct {
	uint8_t control_id[8];       // The ID of the control we are setting the value on
	uint8_t value[8];            // The new value for the control, or the data frame number we will next use
} OSMControl;



#endif

