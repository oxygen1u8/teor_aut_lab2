#include "interface.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>

static int32_t subs_values_in_impl(std::string values, std::string impl)
{
    int32_t result = 1;
    for (uint32_t i = 0; i < values.length(); i++) {
        if (impl[i] == '0') {
            if (values[i] == '0') {
                result &= 1;
            } else if (values[i] == '1') {
                result &= 0;
            } else if (values[i] == '-') {
                continue;
            }
        } else if (impl[i] == '1') {
            if (values[i] == '0') {
                result &= 0;
            } else if (values[i] == '1') {
                result &= 1;
            } else if (values[i] == '-') {
                continue;
            }
        } else if (impl[i] == '-') {
            continue;
        }
    }
    return result;
} 

static std::vector<int32_t> parse_string(std::string &str)
{
    uint32_t index = 0;
    char buff[1024];
    std::vector<int32_t> out;
    memset(buff, 0, sizeof(buff));

    for (uint32_t i = 0; i < str.length() + 1; i++, index++) {
        if (i > 0 && str[i - 1] == ' ') {
            index = 0;
            memset(buff, 0, sizeof(buff));
        }
        if (i == str.length()) {
            if (buff[0] == '-') {
                out.push_back(-1);
            } else {
                out.push_back(atoi(buff));
            }
            break;
        }
        if (str[i] != ' ') {
            buff[index] = str[i];
        } else {
            if (buff[0] == '-') {
                out.push_back(-1);
            } else {
                out.push_back(atoi(buff));
            }
        }
    }

    return out;
}

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

    bool is_exist = false;

    for (uint32_t i = 0; i < this->_table->get_row_count(); i++) {
        for (uint32_t j = 0; j < this->_table->get_column_count(); j++) {
            tmp = this->_table->value(i, j);
            is_exist = false;
            if (tmp.item_output() < 0) {
                continue;
            }
            for (uint32_t k = 0; k < this->outputs.size(); k++) {
                if (tmp.item_output() == outputs[k]) {
                    is_exist = true;
                    break;
                }
            }
            if (!is_exist) {
                outputs.push_back(tmp.item_output());
            }
        }
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
                        + this->states[k];
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

    for (uint32_t i = 0; i < this->outputs.size(); i++) {
        for (uint32_t j = 0; j < row_count; j++) {
            for (uint32_t k = 0; k < column_count; k++) {
                tmp_item = this->_table->value(j, k);
                if (tmp_item.item_output() == this->outputs[i]) {
                    tmp_str = int_to_str2(j, this->input_lines_count)
                        + this->states[k];
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
    for (uint32_t i = 0; i < this->d_tg_count; i++) {
        out << "Q" << i << std::endl;
        this->_dnf[i].print(out);
        out << std::endl;
    }

    if (this->_dnf.size() - this->d_tg_count == this->output_lines_count) {
        for (uint32_t i = 0; i < this->output_lines_count; i++) {
            out << "y" << i << std::endl;
            this->_dnf[this->d_tg_count + i].print(out);
            out << std::endl;
        }
    }
}

void struct_synthesis::check(std::ifstream &check_file)
{
    std::string tmp;
    std::vector<int32_t> tmp_vector;

    std::vector<int32_t> input_vector;
    std::vector<int32_t> state_vector;
    std::vector<int32_t> output_vector;

    while (!check_file.eof()) {
        std::getline(check_file, tmp);
        tmp_vector = parse_string(tmp);

        input_vector.push_back(tmp_vector[0]);
        state_vector.push_back(tmp_vector[1]);
        output_vector.push_back(tmp_vector[2]);
    }

    int32_t input;
    int32_t state;
    int32_t output;

    int32_t tmp_val;

    std::vector<std::string> dnf_in_str;
    std::vector<int32_t> values;
    int32_t next_state = 0;
    int32_t check_output = 0;

    for (uint32_t i = 0; i < input_vector.size(); i++) {
        input = input_vector[i];
        state = state_vector[i];
        output = output_vector[i];

        next_state = 0;
        check_output = 0;

        tmp_val = 0;
        tmp = "";
        tmp = int_to_str2(input - 1, this->input_lines_count) + this->states[state - 1];

        for (uint32_t j = 0; j < this->_dnf.size(); j++) {
            dnf_in_str = this->_dnf[j].get_dnf();
            for (uint32_t k = 0; k < dnf_in_str.size(); k++) {
                tmp_val |= subs_values_in_impl(tmp, dnf_in_str[k]);
            }
            values.push_back(tmp_val);
            tmp_val = 0;
        }

        tmp = "";
        for (uint32_t j = 0; j < this->d_tg_count; j++) {
            next_state |= (values[j] << j);
        }

        for (uint32_t j = 0; j < this->output_lines_count; j++) {
            check_output |= (values[this->d_tg_count + j] << j);
        }

        if (check_output == 0) {
            check_output = -1;
        }

        std::cout << "Input = " << input << " State = " << state << std::endl;
        std::cout << "Next state = " << next_state + 1 << " Output = " << check_output
            << ((check_output == output) ? " (Success)" : " (Failed)") << std::endl;

        values.clear(); 
    }
}
