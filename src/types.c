#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "osm/types.h"

/**
 * WARNING: This file makes the *BOLD* assumption that
 * your platform is using standard IEEE 754 floating
 * point values by default for fp types.
 */

OSMFloatBreakdown osm_float_to_break(OSMFloat f)
{
	OSMFloatBreakdown out;

	out.sign = f >> (OSM_FLOAT_EXPO_LEN + OSM_FLOAT_FRAC_LEN);
	out.mantissa = (f >> OSM_FLOAT_FRAC_LEN) & OSM_FLOAT_EXPO_MASK;
	out.fraction = f & OSM_FLOAT_FRAC_MASK;

	return out;
}

/// Function only to be used with number formats who's mantissa
/// and fraction length are both smaller or equal to double
/// precision IEEE 754 numbers
OSMFloatBreakdown _osm_ieee754_enlarge(uint64_t d, uint16_t m_len, uint16_t f_len)
{
	OSMFloatBreakdown out = {0};

	out.sign = (d >> (m_len + f_len)) & 1;

	uint64_t mask = (1 << (m_len)) - 1; // Mantissa mask
	out.mantissa = (d >> f_len) & mask; // Mantissa biased by the other type's bias
	uint16_t bias = mask >> 1;          // The other type's bias
	
	mask = (1 << f_len) - 1;
	out.fraction = d & mask;
	
	// Create corrected mantissa and fraction (if not denormal)
	if (out.mantissa >= bias)
	{
		// non-negative exponent
		out.mantissa -= bias;
		out.mantissa += OSM_FLOAT_EXPO_BIAS;
	}
	else if (out.mantissa < bias && out.mantissa > 0)
	{
		// true exponent (abs value)
		out.mantissa = bias - out.mantissa;
		// corrected for double precision numbers (won't overflow since we use this function with smaller mantissa)
		out.mantissa = OSM_FLOAT_EXPO_BIAS - out.mantissa;
		// corrected fraction based on difference in bit lengths
		out.fraction = out.fraction << (OSM_FLOAT_FRAC_LEN - f_len);
	}
	
	// Handle zero and denormal
	if (out.mantissa == 0)
	{
		if (out.fraction == 0)
			return out;
		
		// Denormal numbers (TODO)
		out.mantissa = OSM_FLOAT_EXPO_BIAS - bias;
		if (OSM_FLOAT_FRAC_LEN)
			out.fraction = out.fraction << (OSM_FLOAT_FRAC_LEN - f_len);  // corrected fraction based on difference in bit lengths
		bias = (uint16_t) ceil(log2(out.fraction));
		
	}

	return out;
}

/*
 * WARNING: This code assumes that the compiler
 * supports IEEE 754 floating point numbers.
 *
 * Should probably be updated with more formats if I was
 * feeling frisky.
 *
 * Supports half, single, and double precision IEEE 754
 */
OSMFloatBreakdown osm_native_float_to_break(double d)
{
	OSMFloatBreakdown out = {0};

	switch(sizeof(double))
	{
		case 2: {
			uint16_t bits = *(uint16_t *) &d;
			out = _osm_ieee754_enlarge(bits, 5, 10);
		} break;

		case 4: {
			uint32_t bits = * (uint32_t *) &d;
			out = _osm_ieee754_enlarge(bits, 8, 23);
		} break;

		case 8: {
			uint64_t bits = * (uint64_t *) &d;
			out = osm_float_to_break(bits);
		} break;

		default:
			fprintf(stderr, "\nERROR libopensmarts: unable to convert float from double to OSMFloat.\n");
			break;
	}

	return out;
}

OSMFloat osm_break_to_float(OSMFloatBreakdown b)
{
	OSMFloat out;
	
	if (b.sign)
		out = 1;
	out = out << (OSM_FLOAT_FRAC_LEN + OSM_FLOAT_EXPO_LEN);

	out |= (OSMFloat)(b.mantissa & OSM_FLOAT_EXPO_MASK) << OSM_FLOAT_FRAC_LEN;

	out |= b.fraction & OSM_FLOAT_FRAC_MASK;

	return out;
}

bool _osm_is_nan(const OSMFloatBreakdown b)
{
	return b.mantissa == OSM_FLOAT_EXPO_MASK && b.fraction != 0;
}

int8_t _osm_is_infinity(const OSMFloatBreakdown b)
{
	if (b.mantissa == OSM_FLOAT_EXPO_MASK && b.fraction == 0)
	{
		if (b.sign)
			return -1;
		return 1;
	}
	return 0;
}

/// Handle NaN
double _osm_ieee754_nan(OSMFloatBreakdown b, uint16_t m_len, uint16_t f_len)
{
	uint64_t out = 0;
	uint64_t mask = (1 << m_len) - 1;

	// sign
	out |= b.sign << (m_len + f_len);
	// mantissa
	out |= mask << f_len;
	
	// fraction
	if (f_len >= OSM_FLOAT_FRAC_LEN)
		out |= b.fraction << (f_len - OSM_FLOAT_FRAC_LEN);
	else
		out |= b.fraction >> (OSM_FLOAT_FRAC_LEN - f_len);

	// Make sure it's still nan just in case all the
	// fraction bits got cleared
	mask = (1 << f_len) - 1;
	if ((out & mask) == 0)
		out |= 1;

#ifdef _OSM_FLOAT_USE_HIGH_BITS
	out = out << (64 - (m_len + f_len + 1));
#endif
	return *(double *)&out;
}

/// Handle subnormal numbers
double _osm_ieee754_subnormal(OSMFloatBreakdown b, uint16_t m_len, uint16_t f_len)
{
	// TODO
	uint64_t out = 0;
#ifdef _OSM_FLOAT_USE_HIGH_BITS
	out = out << (64 - (m_len + f_len + 1));
#endif
	return *(double *)&out;
}

/**
 * Converts a breakdown to a floating point number with given mantissa and
 * fraction length (so long as the format is less than or equal to 64 bits)
 */
double _osm_ieee754_assemble(OSMFloatBreakdown b, uint16_t m_len, uint16_t f_len)
{

	if (_osm_is_nan(b))
		return _osm_ieee754_nan(b, m_len, f_len);
	else if (b.mantissa == 0)
		return _osm_ieee754_subnormal(b, m_len, f_len);

	uint64_t out = 0;
	
	// Sign (assumed to be at end)
	out |= b.sign << (m_len + f_len);
	
	// Mantissa mask
	uint64_t mask = (1 << m_len) - 1;
	
	// Handle mantissa
	uint64_t bias = mask >> 1;
	if (b.mantissa > OSM_FLOAT_EXPO_BIAS)
	{
		b.mantissa -= OSM_FLOAT_EXPO_BIAS;
		b.mantissa += bias;
		if (b.mantissa >= mask)
		{
			// Create infinity
			out |= mask << f_len;
#ifdef _OSM_FLOAT_USE_HIGH_BITS
			out = out << (64 - (m_len + f_len + 1));
#endif
			return *(double *)&out;
		}
	}
	else
	{
		b.mantissa = OSM_FLOAT_EXPO_BIAS - b.mantissa;
		b.mantissa = bias - b.mantissa;

		if (b.mantissa >= mask || b.mantissa == 0)
		{
			// wrapped around, should create subnormal
			return _osm_ieee754_subnormal(b, m_len, f_len);
		}
	}

	// write mantissa
	out |= (b.mantissa & mask) << f_len;

	// Fraction (assumed to be low bits)
	mask = (1 << f_len) - 1;

	if (f_len >= OSM_FLOAT_FRAC_LEN)
		out |= b.fraction << (f_len - OSM_FLOAT_FRAC_LEN);
	else
		out |= b.fraction >> (OSM_FLOAT_FRAC_LEN - f_len);

#ifdef _OSM_FLOAT_USE_HIGH_BITS
	out = out << (64 - (m_len + f_len + 1));
#endif

	return *(double *)&out;
}

/*
 * WARNING: This code assumes that the compiler
 * supports IEEE 754 floating point numbers.
 *
 * WARNING: This code flushes denormal values to zero
 *
 * WARNING: This code flushes out of bounds values to infinity
 *
 * INFO: The fraction part of NaN values are set to 1 in some cases
 *
 * Should probably be updated with more formats if I was
 * feeling frisky.
 */
double osm_break_to_native_float(OSMFloatBreakdown b)
{
	switch(sizeof(double))
	{
		case 2: {
			return _osm_ieee754_assemble(b, 5, 10);
		} break;
		
		case 4: {
			return _osm_ieee754_assemble(b, 8, 23);
		} break;
		
		case 8: {
			OSMFloat f = osm_break_to_float(b);
			return * (double *) &f;
		} break;
		
		default:
			fprintf(stderr, "\nERROR libopensmarts: unable to convert float from OSMFloat to double.\n");
			break;
	}

	return 0;
}

double osm_float_to_native(OSMFloat f)
{
	return
		osm_break_to_native_float(
			osm_float_to_break(f)
		);
}

OSMFloat osm_native_to_float(double d)
{
	return
		osm_break_to_float(
			osm_native_float_to_break(d)
		);
}

bool osm_is_nan(OSMFloat f)
{
	return _osm_is_nan(osm_float_to_break(f));
}

int8_t osm_is_infinity(OSMFloat f)
{
	return _osm_is_infinity(osm_float_to_break(f));
}

OSMColor osm_rgb_to_color(uint8_t r, uint8_t g, uint8_t b)
{
	OSMColor out = {
		.r = r,
		.g = g,
		.b = b,
		.extra = 0,
		.ex = NULL,
		.ex_names = NULL
	};
	return out;
}

OSMColor osm_color_copy(const OSMColor *color)
{

	OSMColor out = {
		.r = color->r,
		.g = color->g,
		.b = color->b,
		.extra = color->extra,
		.ex = malloc(color->extra),
		.ex_names = malloc(color->extra * sizeof(uint8_t *))
	};

	for (uint8_t i = 0; i < color->extra; i++)
	{
		// char should be just one byte long anyways
		size_t len = strlen((char *)color->ex_names[i]) + 1;
		uint8_t *buf = malloc(strlen((char *)color->ex_names[i]) + 1);
		memcpy(buf, color->ex_names[i], len + 1);
		out.ex_names[i] = buf;
	}

	return out;
}

void osm_color_free(OSMColor *color)
{
	color->r = 0;
	color->g = 0;
	color->b = 0;

	for (uint8_t i = 0; i < color->extra; i++)
	{
		free(color->ex_names[i]);
	}

	free(color->ex_names);
	free(color->ex);

	color->ex_names = NULL;
	color->ex = NULL;

	color->extra = 0;
}
