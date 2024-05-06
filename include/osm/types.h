#ifndef OSM_TYPES_H
#define OSM_TYPES_H

#include <osm/utils.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Defines all types which can be transfered reliably across
 * a device connection
 */

// Raw data
typedef struct {
	uint32_t length;
} OSMRaw;



// Bools

#define OSMTrue 1
#define OSMFalse 0

/// Represents a bool
typedef uint8_t OSMBool;



// Numeric

/// Represents an integer
typedef int64_t OSMInteger;
/// Represents a floating point (IEEE 754 64 bit)
typedef uint64_t OSMFloat;

#define OSM_FLOAT_EXPO_LEN 11
#define OSM_FLOAT_EXPO_MASK 0x7ff
#define OSM_FLOAT_EXPO_BIAS 0x3ff
#define OSM_FLOAT_FRAC_LEN 52
#define OSM_FLOAT_FRAC_MASK 0xffffffffffffff

/// Represents the broken down floating point number
typedef struct {
	uint8_t sign;
	uint16_t mantissa;
	uint64_t fraction;
} OSMFloatBreakdown;

/// Break a OSMFloat into it's constituant parts
OSMFloatBreakdown osm_float_to_break(OSMFloat f);
/// Break a native float into constituant parts
OSMFloatBreakdown osm_native_float_to_break(double d);

/// Convert a breakdown to a float
OSMFloat osm_break_to_float(OSMFloatBreakdown b);
/// Convert a breakdown to a native float
double osm_break_to_native_float(OSMFloatBreakdown b);

/// Convert an osm float to a native one
double osm_float_to_native(OSMFloat f);
/// Convert a native float to an osm one
OSMFloat osm_native_to_float(double d);

/// Check whether a float is NaN
bool osm_is_nan(OSMFloat f);

/// Check whether a float is infinity
/// if positive infinity, returns 1
/// if negative infinity, returns -1
/// otherwise returns 0
int8_t osm_is_infinity(OSMFloat f);



// Color

typedef struct {
	uint8_t val;
	uint8_t *name;
} OSMColorChannel;

/// Color struct with support for extra channels (up to 255)
/// each channel has a range from 0-255
typedef struct {
	uint8_t
		r,
		g,
		b;
	Vector
		extra;
} OSMColor;

/// Output color struct from 24-bit RGB
OSMColor osm_rgb_to_color(uint8_t r, uint8_t g, uint8_t b);

/// Deep copy a color struct
OSMColor osm_color_copy(OSMColor *color);

/// Free a color struct
void osm_color_free(OSMColor *color);

/// Add an extra channel to a color
void osm_add_channel(OSMColor *color, uint8_t val, uint8_t *name);


// Selection

typedef struct {
	/// TODO: Should getting selection options be an async operation?
} OSMSelect;



// TODO: Time, others


#endif
