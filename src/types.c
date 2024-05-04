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

	out.sign = f >> 63;
	out.mantissa = (f >> 52) & 0x7ff;
	out.fraction = f & 0xfffffffffffff;

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
	
	if (out.mantissa >= bias)
	{
		// non-negative exponent
		out.mantissa -= bias;
		out.mantissa += 0x3ff;
	}
	else if (out.mantissa < bias && out.mantissa > 0)
	{
		out.mantissa = bias - out.mantissa;           // true exponent (abs value)
		out.mantissa = 0x3ff - out.mantissa;          // corrected for double precision numbers (won't overflow since we use this function with smaller mantissa)
		out.fraction = out.fraction << (52 - f_len);  // corrected fraction based on difference in bit lengths
	}

	if (out.mantissa == 0)
	{
		if (out.fraction == 0)
			return out;
		
		// Denormal numbers (TODO)
		out.mantissa = 0x3ff - bias;                  // corrected mantissa 
		out.fraction = out.fraction << (52 - f_len);  // corrected fraction based on difference in bit lengths
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
	out = out << 63;

	out |= (OSMFloat)(b.mantissa & 0x7ff) << 52;

	out |= b.fraction & 0xfffffffffffff;

	return out;
}

double _osm_ieee754_assemble(OSMFloatBreakdown b, uint16_t m_len, uint16_t f_len)
{
	double out = 0;
	// Check 
	return out;
}

/*
 * WARNING: This code assumes that the compiler
 * supports IEEE 754 floating point numbers.
 *
 * WARNING: This code flushes denormal values to zero
 *
 * WARNING: This code flushes out of bounds values to infinity
 *
 * INFO: This code keeps NaN values
 *
 * Should probably be updated with more formats if I was
 * feeling frisky.
 */
double osm_break_to_native_float(OSMFloatBreakdown b)
{
	double out = 0;

	switch(sizeof(double))
	{
		case 2: {
		} break;
		
		case 4: {
		} break;
		
		case 8: {
			OSMFloat f = osm_break_to_float(b);
			out = * (double *) &f;
		} break;
		
		default:
			fprintf(stderr, "\nERROR libopensmarts: unable to convert float from OSMFloat to double.\n");
			break;
	}

	if (b.sign)
		out = -out;

	return out;
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
	OSMFloatBreakdown b = osm_float_to_break(f);
	return b.mantissa == 0x3ff && b.fraction != 0;
}

int8_t osm_is_infinity(OSMFloat f)
{
	OSMFloatBreakdown b = osm_float_to_break(f);
	if (b.mantissa == 0x3ff && b.fraction == 0)
	{
		if (b.sign)
			return -1;
		return 1;
	}
	return 0;
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
		memcpy(buf, color->ex_names[i], len);
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
