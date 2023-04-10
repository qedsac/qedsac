/*
 * layout.h -- declarations for Layout namespace
 *             (widget sizes and dimensions)
 */
#ifndef LAYOUT_H
#define LAYOUT_H

#include <QHash>
#include <QRect>

namespace Layout {
    struct Pixels {
        int pixels_per_bit,
            bit_level,
            bit_dy,
            blob_size,
            dial_font,
            button_font,
            output_title_font,
            lt_sel_font,
            // clock stuff
            inner_radius,
            outer_radius,
            minute_hand,
            second_hand,
            font;
    };
    extern const QHash<int,Pixels> SIZE;
    struct Dimens {
        QRect
            clear,
            reset,
            start,
            stop,
            single,
            output_title,
            output,
            lt_sel,
            clock,
            digital_clock,
            dial[10],
            acc,
            mcand,
            mplier,
            sct,
            order,
            store,
            stoplight;
    };
    extern const QHash<int,Dimens> DIMEN;
};

#endif
