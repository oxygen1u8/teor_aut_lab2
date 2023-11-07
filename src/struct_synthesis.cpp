#include "interface.h"
#include <cmath>

struct_synthesis::struct_synthesis(table *synt_table)
{
    this->_table = synt_table;

    /* Поиск количества состояний */
    uint32_t row_count = this->_table->get_row_count();
    uint32_t column_count = this->_table->get_column_count();

    int32_t max_value = 0;
    table_item tmp;

    for (uint32_t i = 0; i < row_count; i++) {
        for (uint32_t j = 0; j < column_count; j++) {
            tmp = this->_table->value(i, j);
            if (max_value < tmp.item_state()) {
                max_value = tmp.item_state();
            }
        }
    }

    this->state_count = max_value;
    this->d_tg_count = (uint32_t) ceil(log2((double) max_value));

    max_value = 0;
    for (uint32_t i = 0; i < row_count; i++) {
        for (uint32_t j = 0; j < column_count; j++) {
            tmp = this->_table->value(i, j);
            if (max_value < tmp.item_output()) {
                max_value = tmp.item_output();
            }
        }
    }

    this->output_lines_count = ((uint32_t) round(log2((double) max_value))) + 1;
}
