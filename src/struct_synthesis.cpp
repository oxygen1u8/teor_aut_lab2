#include "interface.h"
#include <cmath>
#include <algorithm>
#include <iostream>

static std::string int_to_str2(uint32_t value, uint32_t digit)
{
    std::string ret_val = "";
    uint32_t i = 0;
    while (1) {
        if ((value & (1 << i)) == (1 << i)) {
            ret_val += '1';
        } else {
            ret_val += '0';
        }
        i++;
        if (i == digit) {
            break;
        }
    }
    std::reverse(ret_val.begin(), ret_val.end());
    return ret_val;
}

static int32_t str2_to_int(std::string str)
{
    uint32_t length = str.length();
    int32_t ret_val = 0;
    for (uint32_t i = 0; i < length; i++) {
        if (str[i] == '1') {
            ret_val |= (1 << length - i - 1);
        }
    }
    return ret_val;
}

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

    /* Количество D-TG для автомата Мили */
    this->d_tg_count = (uint32_t) ceil(log2((double) max_value));

    /* Поиск количества выходных сигналов */
    max_value = 0;
    for (uint32_t i = 0; i < row_count; i++) {
        for (uint32_t j = 0; j < column_count; j++) {
            tmp = this->_table->value(i, j);
            if (max_value < tmp.item_output()) {
                max_value = tmp.item_output();
            }
        }
    }

    this->input_lines_count = (uint32_t) round(log2((double) row_count));
    this->output_lines_count = ((uint32_t) round(log2((double) max_value))) + 1;

    /* Кодировка состояний */
    for (uint32_t i = 0; i < state_count; i++) {
        this->states.push_back(int_to_str2(i, d_tg_count));
    }
}

void struct_synthesis::sdnf()
{
    std::vector<std::string> impls;
    std::vector<uint32_t> int_impls;
    std::string tmp_str;
    uint32_t mask;
    table_item tmp_item;
    int32_t state_num = 0;
    uint32_t f_len = this->input_lines_count + this->d_tg_count;

    uint32_t row_count = this->_table->get_row_count();
    uint32_t column_count = this->_table->get_column_count();

    for (uint32_t i = 0; i < this->d_tg_count; i++) {
        mask = (1 << i);
        for (uint32_t j = 0; j < row_count; j++) {
            for (uint32_t k = 0; k < column_count; k++) {
                tmp_item = this->_table->value(j, k);
                state_num = tmp_item.item_state();
                if (state_num < 0) {
                    continue;
                }
                if ((state_num - 1) & mask) {
                    tmp_str = int_to_str2(j, this->input_lines_count)
                        + this->states[state_num - 1];
                    impls.push_back(tmp_str);
                }
            }
        }
        for (uint32_t j = 0; j < impls.size(); j++) {
            int_impls.push_back(str2_to_int(impls[j]));
        }
        tmp_str.clear();

        for (uint32_t j = 0; j < (uint32_t) pow((double) 2, (double) f_len); j++) {
            tmp_str += '0';
        }

        for (uint32_t j = 0; j < (uint32_t) int_impls.size(); j++) {
            tmp_str[int_impls[j]] = '1';
        }

        dnf tmp_dnf(tmp_str);
        this->_dnf.push_back(tmp_dnf);
    
        impls.clear();
        int_impls.clear();
        tmp_str.clear();
    }
}

void struct_synthesis::mdnf()
{
    for (uint32_t i = 0; i < this->_dnf.size(); i++) {
        this->_dnf[i].minimize();
    }
}

void struct_synthesis::print(std::ostream &out)
{
    for (uint32_t i = 0; i < this->_dnf.size(); i++) {
        out << "Q" << i << std::endl;
        this->_dnf[i].print(out);
        out << std::endl;
    }
}
