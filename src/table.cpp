#include "interface.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>

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

table::table(std::ifstream &state_file, std::ifstream &output_file)
{
    std::string tmp_state;
    std::string tmp_output;
    while (!state_file.eof()) {
        std::getline(state_file, tmp_state);
        if (this->row_count == 0) {
            for (uint32_t i = 0; i < tmp_state.length(); i++) {
                if (tmp_state[i] == ' ') {
                    this->column_count += 1;
                }
            }
            this->column_count += 1;
        }
        this->row_count += 1;
    }

    state_file.seekg(0);

    this->items = new table_item*[this->row_count];
    for (uint32_t i = 0; i < this->row_count; i++) {
        this->items[i] = new table_item[this->column_count];
    }

    std::vector<int32_t> state_buff;
    std::vector<int32_t> output_buff; 

    for (uint32_t i = 0; i < this->row_count; i++) {
        std::getline(state_file, tmp_state);
        std::getline(output_file, tmp_output);

        state_buff = parse_string(tmp_state);
        output_buff = parse_string(tmp_output);

        for (uint32_t j = 0; j < state_buff.size(); j++) {
            this->items[i][j].set_param(state_buff[j], output_buff[j]);
        }
    }
}

void table::print()
{
    char buff[1024];
    for (uint32_t i = 0; i < this->row_count; i++) {
        for (uint32_t j = 0; j < this->column_count; j++) {
            memset(buff, 0, sizeof(buff));
            sprintf(buff, "%d", this->items[i][j].item_state());
            std::cout << buff << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    for (uint32_t i = 0; i < this->row_count; i++) {
        for (uint32_t j = 0; j < this->column_count; j++) {
            memset(buff, 0, sizeof(buff));
            sprintf(buff, "%d", this->items[i][j].item_output());
            std::cout << buff << ' ';
        }
        std::cout << std::endl;
    }
}

table_item table::value(uint32_t row, uint32_t column) {
    return this->items[row][column];
}

uint32_t table::get_row_count()
{
    return this->row_count;
}

uint32_t table::get_column_count()
{
    return this->column_count;
}

table::~table()
{
    for (uint32_t i = 0; i < this->row_count; i++) {
        delete []this->items[i];
    }
    
    delete this->items;
}
