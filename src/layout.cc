/*
 * layout.cc -- definitions for Layout namespace
 *              (widget sizes and dimensions)
 */
#include "layout.h"

const QHash<int,Layout::Pixels> Layout::SIZE = {
    {83, {      // 83% scale factor
        5,      // pixels_per_bit
        4,      // bit_level
        9,      // bit_dy
        2,      // blob_size
        16,     // dial_font
        9,      // button_font
        11,     // output_title_font
        8,      // lt_sel_font
        20,     // clock inner radius
        24,     // clock outer radius
        13,     // clock minute hand
        19,     // clock second hand
        7,      // digital clock font size
      }
    },
    {100, {     // 100% scale factor
        6,      // pixels_per_bit
        5,      // bit_level
        11,     // bit_dy
        3,      // blob_size
        20,     // dial_font
        12,     // button_font
        14,     // output_title_font
        10,     // lt_sel_font
        25,     // clock inner radius
        30,     // clock outer radius
        16,     // clock minute hand
        24,     // clock second hand
        8,      // digital clock font size
      }
    },
    {133, {     // 133% scale factor
        8,      // pixels_per_bit
        6,      // bit_level
        14,     // bit_dy
        3,      // blob_size
        26,     // dial_font
        15,     // button_font
        18,     // output_title_font
        13,     // lt_sel_font
        33,     // clock inner radius
        39,     // clock outer radius
        21,     // clock minute hand
        31,     // clock second hand
        11,     // digital clock font size
      }
    },
    {150, {     // 150% scale factor
        9,      // pixels_per_bit
        7,      // bit_level
        16,     // bit_dy
        4,      // blob_size
        30,     // dial_font
        18,     // button_font
        21,     // output_title_font
        15,     // lt_sel_font
        37,     // clock inner radius
        45,     // clock outer radius
        24,     // clock minute hand
        36,     // clock second hand
        12,     // digital clock font size
      }
    },
    {200, {     // 200% scale factor
        12,     // pixels_per_bit
        10,     // bit_level
        22,     // bit_dy
        6,      // blob_size
        40,     // dial_font
        24,     // button_font
        28,     // output_title_font
        20,     // lt_sel_font
        50,     // clock inner radius
        60,     // clock outer radius
        32,     // clock minute hand
        48,     // clock second hand
        16,     // digital clock font size
      }
    },
};

const QHash<int,Layout::Dimens> Layout::DIMEN = {
    {83, {                          // 83% scale factor
        {263, 164,  54,  16},       // clear button
        {332, 164,  54,  16},       // reset button
        {263, 184,  54,  16},       // start button
        {332, 184,  54,  16},       // stop button
        {283, 204,  82,  16},       // single e.p. button
        {239,   1, 268,  19},       // output title field
        {239,  19, 268, 130},       // output area (teleprinter)
        {355, 258,  37,  15},       // long tank selector
        {413, 155,  94,  74},       // clock
        { 25,  45,  45,   8},       // digital clock
        {   {450, 296,  17,  18},   // dial button [0]
            {479, 260,  17,  18},   // dial button [1]
            {473, 248,  17,  18},   // dial button [2]
            {461, 240,  17,  18},   // dial button [3]
            {447, 239,  17,  18},   // dial button [4]
            {434, 245,  17,  18},   // dial button [5]
            {425, 256,  17,  18},   // dial button [6]
            {423, 269,  17,  18},   // dial button [7]
            {426, 282,  17,  18},   // dial button [8]
            {436, 292,  17,  18},   // dial button [9]
        },
        { 14, 302, 353,   8},       // accumulator display tube
        { 14, 282, 174,   8},       // multiplicand display tube
        { 14, 262, 174,   8},       // multiplier display tube
        { 14, 243,  49,   8},       // sct display tube
        {103, 243,  84,   8},       // order display tube
        { 31,  43, 174, 144},       // store display
        {267, 245,   5,   5},       // stop light
      }
    },
    {100, {                         // 100% scale factor
        {317, 198,  66,  20},       // clear button
        {401, 198,  66,  20},       // reset button
        {317, 222,  66,  20},       // start button
        {401, 222,  66,  20},       // stop button
        {342, 246,  99,  20},       // single e.p. button
        {288,   2, 324,  23},       // output title field
        {289,  24, 323, 157},       // output area (teleprinter)
        {429, 311,  45,  18},       // long tank selector
        {498, 187, 114,  90},       // clock
        { 29,  54,  57,   9},       // digital clock
        {   {543, 358,  20,  21},   // dial button [0]
            {578, 314,  20,  21},   // dial button [1]
            {570, 300,  20,  21},   // dial button [2]
            {556, 290,  20,  21},   // dial button [3]
            {539, 289,  20,  21},   // dial button [4]
            {523, 296,  20,  21},   // dial button [5]
            {513, 309,  20,  21},   // dial button [6]
            {510, 325,  20,  21},   // dial button [7]
            {514, 341,  20,  21},   // dial button [8]
            {526, 353,  20,  21},   // dial button [9]
        },
        { 17, 363, 426,  10},       // accumulator display tube
        { 17, 339, 210,  10},       // multiplicand display tube
        { 17, 316, 210,  10},       // multiplier display tube
        { 17, 292,  60,  10},       // sct display tube
        {125, 292, 102,  10},       // order display tube
        { 38,  52, 210, 176},       // store display
        {322, 295,   7,   7},       // stop light
      }
    },
    {133, {                         // 133% scale factor
        {421, 263,  87,  26},       // clear button
        {533, 263,  87,  26},       // reset button
        {421, 295,  87,  26},       // start button
        {533, 295,  87,  26},       // stop button
        {454, 327, 131,  26},       // single e.p. button
        {383,   2, 430,  30},       // output title field
        {384,  31, 429, 208},       // output area (teleprinter)
        {570, 413,  59,  23},       // long tank selector
        {662, 248, 151, 119},       // clock
        { 38,  71,  74,  12},       // digital clock
        {   {722, 477,  26,  26},   // dial button [0]
            {768, 418,  26,  26},   // dial button [1]
            {758, 400,  26,  26},   // dial button [2]
            {739, 387,  26,  26},   // dial button [3]
            {716, 385,  26,  26},   // dial button [4]
            {695, 395,  26,  26},   // dial button [5]
            {682, 412,  26,  26},   // dial button [6]
            {678, 433,  26,  26},   // dial button [7]
            {683, 454,  26,  26},   // dial button [8]
            {699, 470,  26,  26},   // dial button [9]
        },
        { 22, 484, 566,  13},       // accumulator display tube
        { 22, 452, 279,  13},       // multiplicand display tube
        { 22, 421, 279,  13},       // multiplier display tube
        { 22, 389,  79,  13},       // sct display tube
        {166, 389, 135,  13},       // order display tube
        { 50,  74, 279, 224},       // store display
        {428, 391,   9,   9},       // stop light
      }
    },
    {150, {                         // 150% scale factor
        {475, 297,  99,  30},       // clear button
        {601, 297,  99,  30},       // reset button
        {475, 333,  99,  30},       // start button
        {601, 333,  99,  30},       // stop button
        {513, 369, 148,  30},       // single e.p. button
        {432,   3, 486,  34},       // output title field
        {433,  36, 484, 235},       // output area (teleprinter)
        {644, 466,  67,  27},       // long tank selector
        {747, 280, 171, 135},       // clock
        { 42,  81,  86,  13},       // digital clock
        {   {814, 538,  30,  30},   // dial button [0]
            {867, 472,  30,  30},   // dial button [1]
            {855, 451,  30,  30},   // dial button [2]
            {834, 436,  30,  30},   // dial button [3]
            {808, 435,  30,  30},   // dial button [4]
            {784, 445,  30,  30},   // dial button [5]
            {769, 465,  30,  30},   // dial button [6]
            {765, 489,  30,  30},   // dial button [7]
            {771, 513,  30,  30},   // dial button [8]
            {789, 531,  30,  30},   // dial button [9]
        },
        { 25, 545, 639,  15},       // accumulator display tube
        { 25, 509, 315,  15},       // multiplicand display tube
        { 25, 474, 315,  15},       // multiplier display tube
        { 25, 438,  90,  15},       // sct display tube
        {187, 438, 153,  15},       // order display tube
        { 57,  79, 315, 256},       // store display
        {482, 441,  10,  10},       // stop light
      }
    },
    {200, {                         // 200% scale factor
        {634, 396, 132,  40},       // clear button
        {802, 396, 132,  40},       // reset button
        {634, 444, 132,  40},       // start button
        {802, 444, 132,  40},       // stop button
        {684, 492, 198,  40},       // single e.p. button
        {576,   4, 648,  46},       // output title field
        {578,  48, 646, 314},       // output area (teleprinter)
        {859, 622,  90,  36},       // long tank selector
        {996, 374, 228, 180},       // clock
        { 56, 108, 115,  18},       // digital clock
        {   {1086, 718,  40,  40},  // dial button [0]
            {1156, 630,  40,  40},  // dial button [1]
            {1140, 602,  40,  40},  // dial button [2]
            {1112, 582,  40,  40},  // dial button [3]
            {1078, 580,  40,  40},  // dial button [4]
            {1046, 594,  40,  40},  // dial button [5]
            {1026, 620,  40,  40},  // dial button [6]
            {1020, 652,  40,  40},  // dial button [7]
            {1028, 684,  40,  40},  // dial button [8]
            {1052, 708,  40,  40},  // dial button [9]
        },
        { 34, 725, 852,  20},       // accumulator display tube
        { 34, 677, 420,  20},       // multiplicand display tube
        { 34, 630, 420,  20},       // multiplier display tube
        { 34, 583, 120,  20},       // sct display tube
        {250, 583, 204,  20},       // order display tube
        { 76, 104, 420, 352},       // store display
        {643, 588,  14,  14},       // stop light
      }
    },
};
