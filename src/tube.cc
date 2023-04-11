/*
 * tube.cc -- definitions for Tube class
 *            (EDSAC display tubes)
 */
#include <QPainter>

#include "tube.h"

#include "attributes.h"
#include "display.h"
#include "edsac.h"
#include "layout.h"
#include "menu.h"
#include "settings.h"
#include "tank.h"

// needs to be pointer; can't intialize QCursor/QPixmap at compile time
static QCursor *uparrow_cursor = nullptr;

Display_Tube::Display_Tube(Tank *t, unsigned nlines, unsigned nbits,
                           bool is_shrt, QWidget *parent,
                           const QString& lbl, unsigned h)
    : QWidget(parent), data(t), num_lines(nlines), num_bits(nbits),
      is_short(is_shrt), label(lbl), hint_flags(h)
{
    if (uparrow_cursor == nullptr) {    // initialize cursor at first call
        //uparrow_curror = new QCursor(Qt::UpArrowCursor);
        // the following looks better:
        QPixmap p(":/uparrow.png");
        uparrow_cursor = new QCursor(p, p.width()/2, 0);
    }
}

// set specified long tank to be shown in display window
// (meaningless for short tank display tubes)
void Display_Tube::set_tank(int i)
{
    if (is_short) { return; }      // (defensive programming)
    data = edsac->store_tank(i);
    display->update();
}

static const QString FUNC_LETTERS = "PQWERTYUIOJπSZK*.FθDΦHNMΔLXGABCV";

// determine an order's suffix, depending on the selected initial orders
inline QChar order_suffix(bool is_short)
{
    if (Settings::initial_orders() == 1) {
        return is_short ? 'S' : 'L';
    } else {
        return is_short ? 'F' : 'D';
    }
}

// translate the order in the specified word into an EDSAC order
inline QString order_to_str(WORD w)
{
    QString func = FUNC_LETTERS.at((w & MAX_HIWORD) >> (ADDR_BITS+FLAG_BITS)),
            suffix = order_suffix((w & MAX_FLAG) == 0);
    unsigned addr = (w >> FLAG_BITS) & MAX_ADDR;
    return func + " " + QString::number(addr) + " " + suffix;
}

// translate the fraction in the specified EDSAC long word into its
// string representation, with the specified precision
QString frac_to_str(WORD low, WORD high, unsigned precision)
{
    LONGWORD x = long_word(low, high);
    LONGWORD mask = LONG_SIGN_BIT >> 1;
    double result = 0.0;
    if ((x & LONG_SIGN_BIT) != 0) { result = -1.0; }
    double addend = 0.5;
    while (mask != 0) {
        if ((x & mask) != 0) { result += addend; }
        addend /= 2;
        mask >>= 1;
    }

    return QString::number(result, 'f', precision);
}

// translate the fraction in the specified EDSAC short word into its
// string representation, with the specified precision
inline QString frac_to_str(WORD w, unsigned precision)
{
    return frac_to_str(0, w, precision);
}

// for short tanks, display the appropriate hints, if required
// for long tanks, prepare to do so when the cursor is in position
void Display_Tube::enterEvent(QEvent *e)
{
    if (Settings::hints()) {
        setCursor(*uparrow_cursor);
        if (is_short) {
            if (Settings::short_tanks()) {
                WORD *p = data->end() - 1;
                QString hint = label;

                if ((hint_flags & AS_ORDER) != 0) {
                    if ((hint_flags & LABEL) != 0) { hint += "   Order = "; }
                    hint += " = " + order_to_str(*p);
                }

                if ((hint_flags & AS_INTEGER) != 0) {
                    if ((hint_flags & LABEL) != 0) { hint += "   Integer"; }
                    hint += " = " + QString::number(sign_extend(*p));
                    if (num_bits > LOWORD_BITS) {
                        hint += QString("[") + order_suffix(true) + "]   ";
                        hint += QString::number(sign_extend(*(p-1), *p))
                                    + "[" + order_suffix(false) + "]";
                    }
                }

                if ((hint_flags & AS_FRAC) != 0) {
                    // LABEL assumed with short tank fractions
                    // 35-bit fractions assumed
                    hint += "   Fraction = ";
                    hint += frac_to_str(*(p-1), *p, 15);
                }

                menu->show_hint(hint);
            }
        } else { setMouseTracking(true); }
    }
}

// clear the last hint from the status bar
void Display_Tube::leaveEvent(QEvent *e)
{
    if (Settings::hints()) {
        setCursor(Qt::ArrowCursor);
        setMouseTracking(false);
        menu->clear_hint();
    }
}

// for the currently displayed long tank, display the appropriate
// hints for the EDSAC word under the mouse cursor, if required 
void Display_Tube::mouseMoveEvent(QMouseEvent *e)
{   // must be long tank!
    QString hint = label + " ";
    unsigned line = num_lines - (e->y() / (height() / num_lines) + 1);
    unsigned base = Settings::long_tank() * LONG_TANK_WORDS;
    bool odd = e->x() < width()/2;
    unsigned offset = line*2 + int(odd);
    unsigned addr = base + offset;
    WORD *p = data->begin(offset);

    hint += QString::number(addr);
    hint += "   Order = ";
    hint += order_to_str(*p);

    hint += "   Integer ";
    hint += QString::number(addr) + order_suffix(true) + " = ";
    hint += QString::number(sign_extend(*p));

    hint += "   Fraction ";

    if (odd) {
        hint += QString::number(addr & ~0x1) + order_suffix(false) + " = ";
        hint += frac_to_str(*(p-1), *p, 15);
    } else {
        hint += QString::number(addr) + order_suffix(true) + " = ";
        // & MAX_HIWORD ???
        hint += frac_to_str(*p, 6);
    }

    menu->show_hint(hint);
}

void Display_Tube::paintEvent(QPaintEvent *e)
{
using namespace Layout;

    QPainter qp(this);
    qp.setRenderHint(QPainter::Antialiasing);
    qp.setPen(Qt::green);
    qp.setBrush(QBrush(Qt::green));
    if (is_short and not Settings::short_tanks()) {
        // note that all short tank tubes have only 1 line
        int dx = SIZE.value(Settings::scale_factor()).pixels_per_bit;
        int blob_size = SIZE.value(Settings::scale_factor()).blob_size/2;
        int y = SIZE.value(Settings::scale_factor()).bit_level;
        for (int i = 0, x = 2; i < num_bits; i++, x += dx) {
            qp.drawEllipse(x, y, blob_size, blob_size);
        }
        return;
    }
    int dx = SIZE.value(Settings::scale_factor()).pixels_per_bit,
        dy = SIZE.value(Settings::scale_factor()).bit_dy;
    QRect r = e->rect();
    int nlines = is_short ? num_lines : r.height() / dy;
    int start_word = is_short ? 0
                              : 2 * (num_lines - (r.top() + r.height()) / dy);
    int start_x = 2 + (SIZE.value(Settings::scale_factor()).pixels_per_bit
            * (num_bits - 1)),
        //start_y = (num_lines - 1) * dy
        start_y = r.top() + (nlines - 1) * dy
            + SIZE.value(Settings::scale_factor()).bit_level,
        end_y = r.top();
    int blob_size = SIZE.value(Settings::scale_factor()).blob_size;
    auto p = data->begin(start_word);
    for (int y = start_y; y > end_y; y -= dy) {
        WORD value = *p;
        for (int j = 0, x = start_x; j < num_bits; j++, x -= dx) {
            if ((j % LOWORD_BITS) == 0 and j != 0) {
                value = *++p;
            }
            if ((value & 0x1) != 0) {
                qp.drawEllipse(x-1, y-1, blob_size, blob_size);
            } else {
                qp.drawEllipse(x, y, blob_size/2, blob_size/2);
            }
            value >>= 1;
        }
        p++;
    }
}
