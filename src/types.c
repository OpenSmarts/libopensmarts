#include <math.h>

#include "osm/types.h"

OSMFloatBreakdown osm_float_to_break(OSMFloat f)
{
	OSMFloatBreakdown out;

	out.sign = f >> 63;
	out.mantissa = (f >> 52) & 0x7ff;
	out.fraction = f & 0xfffffffffffff;

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
			out.sign = bits >> 15;
		} break;

		case 4: {
			uint32_t bits = * (uint32_t *) &d;
			out.sign = bits >> 31;
		} break;

		case 8: {
			uint64_t bits = * (uint64_t *) &d;
			out = osm_float_to_break(bits);
		} break;

		default:
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

/*
 * WARNING: This code assumes that the compiler
 * supports IEEE 754 floating point numbers.
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
