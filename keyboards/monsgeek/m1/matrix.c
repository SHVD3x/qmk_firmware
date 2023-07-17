/*
Copyright 2023 SHVD3x
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdbool.h>
#include "wait.h"
//#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"

//#define rows_bits   PAL_PORT_BIT(PAL_PAD(C1)) | PAL_PORT_BIT(PAL_PAD(C2)) | PAL_PORT_BIT(PAL_PAD(C3)) | PAL_PORT_BIT(PAL_PAD(A0)) | PAL_PORT_BIT(PAL_PAD(A1)) | PAL_PORT_BIT(PAL_PAD(A2)) | PAL_PORT_BIT(PAL_PAD(A3)) | PAL_PORT_BIT(PAL_PAD(A4)) | PAL_PORT_BIT(PAL_PAD(A5)) | PAL_PORT_BIT(PAL_PAD(A6)) | PAL_PORT_BIT(PAL_PAD(A7)) | PAL_PORT_BIT(PAL_PAD(C4)) | PAL_PORT_BIT(PAL_PAD(C5)) | PAL_PORT_BIT(PAL_PAD(B0)) | PAL_PORT_BIT(PAL_PAD(B1)) | PAL_PORT_BIT(PAL_PAD(B2))
//#define cols_bits   PAL_PORT_BIT(PAL_PAD(B15)) | PAL_PORT_BIT(PAL_PAD(C6)) | PAL_PORT_BIT(PAL_PAD(C7)) | PAL_PORT_BIT(PAL_PAD(C8)) | PAL_PORT_BIT(PAL_PAD(C9)) | PAL_PORT_BIT(PAL_PAD(A8))

#define rowA_bits   PAL_PORT_BIT(PAL_PAD(A0)) | PAL_PORT_BIT(PAL_PAD(A1)) | PAL_PORT_BIT(PAL_PAD(A2)) | PAL_PORT_BIT(PAL_PAD(A3)) | PAL_PORT_BIT(PAL_PAD(A4)) | PAL_PORT_BIT(PAL_PAD(A5)) | PAL_PORT_BIT(PAL_PAD(A6)) | PAL_PORT_BIT(PAL_PAD(A7))
#define rowB_bits   PAL_PORT_BIT(PAL_PAD(B0)) | PAL_PORT_BIT(PAL_PAD(B1)) | PAL_PORT_BIT(PAL_PAD(B2))
#define rowC_bits   PAL_PORT_BIT(PAL_PAD(C1)) | PAL_PORT_BIT(PAL_PAD(C2)) | PAL_PORT_BIT(PAL_PAD(C3)) | PAL_PORT_BIT(PAL_PAD(C4)) | PAL_PORT_BIT(PAL_PAD(C5))

#define colA_bits   PAL_PORT_BIT(PAL_PAD(A8))
#define colB_bits   PAL_PORT_BIT(PAL_PAD(B15))
#define colC_bits   PAL_PORT_BIT(PAL_PAD(C6)) | PAL_PORT_BIT(PAL_PAD(C7)) | PAL_PORT_BIT(PAL_PAD(C8)) | PAL_PORT_BIT(PAL_PAD(C9))

/* matrix state(1:on, 0:off) */
extern matrix_row_t raw_matrix[MATRIX_ROWS];  // raw values
extern matrix_row_t matrix[MATRIX_ROWS];      // debounced values
// matrix code
// ultra fast read_cols code
static inline matrix_row_t read_cols(void) {
    return ((((palReadPort(PAL_PORT(B0)) >> 15) & 1U) ^  1U) |
           (((palReadPort(PAL_PORT(C0)) >> 5) & (((1U << 4) - 1U) << 1))  ^ (((1U << 4) - 1U) << 1)) | 
            (((palReadPort(PAL_PORT(A0)) >> 3) & (1U << 5)) ^ (1U << 5)));
}

//Посмотреть есть ли команды для считывания или установки режима порта сразу на нескольких портах
//Возможно из матрицы сканирования можно убрать некоторые пины которые не используются тк тут ansi без кнопок iso

static inline void unselect_rows(void) { // Возможно есть смысл сделать функцию unselect_row с условием на порт, но хз
    ATOMIC_BLOCK_FORCEON {
    palSetPort(PAL_PORT(A0), rowA_bits);
    palSetPort(PAL_PORT(B0), rowB_bits);
    palSetPort(PAL_PORT(C0), rowC_bits);
    }
}

//попробовать все реализовать без вспомогательных функций напрямую в функции сканирования, возможно так удастся избежать двойного вычисления бита и порта колонны при select и unselect (если оно вообще есть)
/*
static inline void unselect_row(uint8_t row) {
    if (row != 12)
        ATOMIC_BLOCK_FORCEON {
        palSetPort(PAL_PORT(row_pins[row]), PAL_PORT_BIT(PAL_PAD(row_pins[row])));
        }
    else
        ATOMIC_BLOCK_FORCEON {
        pal_lld_setport(PAL_PORT(B0), rowB_bits);
        pal_lld_setport(PAL_PORT(C0), rowC_bits);
        }
}
*/

//протестить и сравнить что быстрее работает
/*
static inline void unselect_row(uint8_t row) {
    if ((row >= 3) && (row <= 10))
        ATOMIC_BLOCK_FORCEON {
        pal_lld_setport(PAL_PORT(A0), rowA_bits);
        }
    else
        if ((row <= 2) || ((row >= 11) && (row <= 12)))
            ATOMIC_BLOCK_FORCEON {
            pal_lld_setport(PAL_PORT(C0), rowC_bits);
            }
        else
            pal_lld_setport(PAL_PORT(B0), rowB_bits);
}
*/

/*
static inline void select_row(uint8_t row) {
    ATOMIC_BLOCK_FORCEON {
    pal_lld_clearport(PAL_PORT(B0), PAL_PORT_BIT(row));
    }
}
*/

static pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS; 
// возможно из MATRIX_ROW_PINS не получится напрямую считывать пины, тогда использовать константу выше

static inline void select_row(uint8_t row) {
    if (row != 12)
        ATOMIC_BLOCK_FORCEON {
            palClearPort(PAL_PORT(row_pins[row]), PAL_PORT_BIT(PAL_PAD(row_pins[row])));
        }
    else
        ATOMIC_BLOCK_FORCEON {
            palClearPort(PAL_PORT(C5), PAL_PORT_BIT(PAL_PAD(C5)));
            palClearPort(PAL_PORT(B1), PAL_PORT_BIT(PAL_PAD(B1)));
        }
}

// Only need to init the pins. Debounce / raw matrix are initialized already for us.
void matrix_init_custom(void) {
    // initialize key pins
    ATOMIC_BLOCK_FORCEON {
        palSetGroupMode(PAL_PORT(A0),  rowA_bits, 0U, (PAL_WB32_MODE_OUTPUT | PAL_WB32_OTYPE_PUSHPULL | PAL_WB32_OSPEED_HIGH | PAL_WB32_CURRENT_LEVEL3));
        palSetGroupMode(PAL_PORT(B0),  rowB_bits, 0U, (PAL_WB32_MODE_OUTPUT | PAL_WB32_OTYPE_PUSHPULL | PAL_WB32_OSPEED_HIGH | PAL_WB32_CURRENT_LEVEL3));
        palSetGroupMode(PAL_PORT(C0),  rowC_bits, 0U, (PAL_WB32_MODE_OUTPUT | PAL_WB32_OTYPE_PUSHPULL | PAL_WB32_OSPEED_HIGH | PAL_WB32_CURRENT_LEVEL3));
        palSetPort(PAL_PORT(A0),   rowA_bits);
        palSetPort(PAL_PORT(B0),   rowB_bits);
        palSetPort(PAL_PORT(C0),   rowC_bits);
        palSetGroupMode(PAL_PORT(A0),  colA_bits, 0U, (PAL_WB32_MODE_INPUT | PAL_WB32_PUPDR_PULLUP | PAL_WB32_OSPEED_HIGH));
        palSetGroupMode(PAL_PORT(B0),  colB_bits, 0U, (PAL_WB32_MODE_INPUT | PAL_WB32_PUPDR_PULLUP | PAL_WB32_OSPEED_HIGH));
        palSetGroupMode(PAL_PORT(C0),  colC_bits, 0U, (PAL_WB32_MODE_INPUT | PAL_WB32_PUPDR_PULLUP | PAL_WB32_OSPEED_HIGH));
    }
}

// Only need to scan the result into current_matrix, and return changed.
//Experimental adaptive latency scan
uint8_t matrix_scan_custom(matrix_row_t current_matrix[]) { //bool в исходнике, но почему-то uint8_t работает быстрее, хз почему
    bool changed = false;

    // Set col, read rows
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {

        select_row(current_row);
        matrix_output_select_delay();
        
        matrix_row_t cols = read_cols();

        unselect_rows();

        changed |= (current_matrix[current_row] != cols);
        current_matrix[current_row] = cols;
        /*
        while (((((PAL_PORT(B0))->IDR >> 15) & 1U) ^  1U) |
           (((((PAL_PORT(C0))->IDR) >> 5) & (((1U << 4) - 1U) << 1))  ^ (((1U << 4) - 1U) << 1)) | 
            (((((PAL_PORT(A0))->IDR) >> 3) & (1U << 5)) ^ (1U << 5)))   // Wait for all Col signals to go HIGH
        */
        while ( ( (palReadPort(PAL_PORT(B0)) & (1U << 15)) |  //Перепроверить
           (palReadPort(PAL_PORT(C0)) & (((1U << 4) - 1U) << 6)) | 
            (palReadPort(PAL_PORT(A0)) & (1U << 8)) ) != ((1U << 15) |  (((1U << 4) - 1U) << 6) | (1U << 8)) )    // Wait for all Col signals to go HIGH

            wait_cpuclock(1);
    }

    return changed;
}
