/* 
 * File:   ResValue.h
 * Author : angelToms
 *
 */

#ifndef RESVALUE_H
#define	RESVALUE_H

#include <stdint.h>

#include "../ByteOrder.h"
#include "../QupLog.h"

/**
 * Representation of a value in a resource, supplying type
 * information.
 */
struct Res_value {
    // Number of bytes in this structure.
    uint16_t size;

    // Always set to 0.
    uint8_t res0;

    // Type of the data value.

    enum {
        // Contains no data.
        TYPE_NULL = 0x00,
        // The 'data' holds a ResTable_ref, a reference to another resource
        // table entry.
        TYPE_REFERENCE = 0x01,
        // The 'data' holds an attribute resource identifier.
        TYPE_ATTRIBUTE = 0x02,
        // The 'data' holds an index into the containing resource table's
        // global value string pool.
        TYPE_STRING = 0x03,
        // The 'data' holds a single-precision floating point number.
        TYPE_FLOAT = 0x04,
        // The 'data' holds a complex number encoding a dimension value,
        // such as "100in".
        TYPE_DIMENSION = 0x05,
        // The 'data' holds a complex number encoding a fraction of a
        // container.
        TYPE_FRACTION = 0x06,

        // Beginning of integer flavors...
        TYPE_FIRST_INT = 0x10,

        // The 'data' is a raw integer value of the form n..n.
        TYPE_INT_DEC = 0x10,
        // The 'data' is a raw integer value of the form 0xn..n.
        TYPE_INT_HEX = 0x11,
        // The 'data' is either 0 or 1, for input "false" or "true" respectively.
        TYPE_INT_BOOLEAN = 0x12,

        // Beginning of color integer flavors...
        TYPE_FIRST_COLOR_INT = 0x1c,

        // The 'data' is a raw integer value of the form #aarrggbb.
        TYPE_INT_COLOR_ARGB8 = 0x1c,
        // The 'data' is a raw integer value of the form #rrggbb.
        TYPE_INT_COLOR_RGB8 = 0x1d,
        // The 'data' is a raw integer value of the form #argb.
        TYPE_INT_COLOR_ARGB4 = 0x1e,
        // The 'data' is a raw integer value of the form #rgb.
        TYPE_INT_COLOR_RGB4 = 0x1f,

        // ...end of integer flavors.
        TYPE_LAST_COLOR_INT = 0x1f,

        // ...end of integer flavors.
        TYPE_LAST_INT = 0x1f
    };
    uint8_t dataType;

    // Structure of complex data values (TYPE_UNIT and TYPE_FRACTION)

    enum {
        // Where the unit type information is.  This gives us 16 possible
        // types, as defined below.
        COMPLEX_UNIT_SHIFT = 0,
        COMPLEX_UNIT_MASK = 0xf,

        // TYPE_DIMENSION: Value is raw pixels.
        COMPLEX_UNIT_PX = 0,
        // TYPE_DIMENSION: Value is Device Independent Pixels.
        COMPLEX_UNIT_DIP = 1,
        // TYPE_DIMENSION: Value is a Scaled device independent Pixels.
        COMPLEX_UNIT_SP = 2,
        // TYPE_DIMENSION: Value is in points.
        COMPLEX_UNIT_PT = 3,
        // TYPE_DIMENSION: Value is in inches.
        COMPLEX_UNIT_IN = 4,
        // TYPE_DIMENSION: Value is in millimeters.
        COMPLEX_UNIT_MM = 5,

        // TYPE_FRACTION: A basic fraction of the overall size.
        COMPLEX_UNIT_FRACTION = 0,
        // TYPE_FRACTION: A fraction of the parent size.
        COMPLEX_UNIT_FRACTION_PARENT = 1,

        // Where the radix information is, telling where the decimal place
        // appears in the mantissa.  This give us 4 possible fixed point
        // representations as defined below.
        COMPLEX_RADIX_SHIFT = 4,
        COMPLEX_RADIX_MASK = 0x3,

        // The mantissa is an integral number -- i.e., 0xnnnnnn.0
        COMPLEX_RADIX_23p0 = 0,
        // The mantissa magnitude is 16 bits -- i.e, 0xnnnn.nn
        COMPLEX_RADIX_16p7 = 1,
        // The mantissa magnitude is 8 bits -- i.e, 0xnn.nnnn
        COMPLEX_RADIX_8p15 = 2,
        // The mantissa magnitude is 0 bits -- i.e, 0x0.nnnnnn
        COMPLEX_RADIX_0p23 = 3,

        // Where the actual value is.  This gives us 23 bits of
        // precision.  The top bit is the sign.
        COMPLEX_MANTISSA_SHIFT = 8,
        COMPLEX_MANTISSA_MASK = 0xffffff
    };

    // The data for this item, as interpreted according to dataType.
    uint32_t data;

};

extern "C" {

    inline void copyFrom_dtoh(const Res_value& src, Res_value& dst) {
        dst.size = dtohs(src.size);
        dst.res0 = src.res0;
        dst.dataType = src.dataType;
        dst.data = dtohl(src.data);
    }

    __inline__ void print_complex(uint32_t complex, bool isFraction) {
        const float MANTISSA_MULT =
                1.0f / (1 << Res_value::COMPLEX_MANTISSA_SHIFT);
        const float RADIX_MULTS[] = {
            1.0f * MANTISSA_MULT, 1.0f / (1 << 7) * MANTISSA_MULT,
            1.0f / (1 << 15) * MANTISSA_MULT, 1.0f / (1 << 23) * MANTISSA_MULT
        };

        float value = (complex & (Res_value::COMPLEX_MANTISSA_MASK
                << Res_value::COMPLEX_MANTISSA_SHIFT))
                * RADIX_MULTS[(complex >> Res_value::COMPLEX_RADIX_SHIFT)
                & Res_value::COMPLEX_RADIX_MASK];
        QUP_LOGI("[*] %f", value);

        if (!isFraction) {
            switch ((complex >> Res_value::COMPLEX_UNIT_SHIFT) & Res_value::COMPLEX_UNIT_MASK) {
                case Res_value::COMPLEX_UNIT_PX: QUP_LOGI("[*] px");
                    break;
                case Res_value::COMPLEX_UNIT_DIP: QUP_LOGI("[*] dp");
                    break;
                case Res_value::COMPLEX_UNIT_SP: QUP_LOGI("[*] sp");
                    break;
                case Res_value::COMPLEX_UNIT_PT: QUP_LOGI("[*] pt");
                    break;
                case Res_value::COMPLEX_UNIT_IN: QUP_LOGI("[*] in");
                    break;
                case Res_value::COMPLEX_UNIT_MM: QUP_LOGI("[*] mm");
                    break;
                default: QUP_LOGI("[*]  (unknown unit)");
                    break;
            }
        } else {
            switch ((complex >> Res_value::COMPLEX_UNIT_SHIFT) & Res_value::COMPLEX_UNIT_MASK) {
                case Res_value::COMPLEX_UNIT_FRACTION: QUP_LOGI("[*] %%");
                    break;
                case Res_value::COMPLEX_UNIT_FRACTION_PARENT: QUP_LOGI("[*] %%p");
                    break;
                default: QUP_LOGI("[*]  (unknown unit)");
                    break;
            }
        }
    }
}
#endif	/* RESVALUE_H */

