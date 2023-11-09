#ifndef INTERFACE_H
#define INTERFACE_H

#include "types.h"
#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>

class table_item {
    private:
        int32_t state;
        int32_t output;
    public:
        table_item();
        table_item(int32_t state, int32_t output);
        ~table_item();
        void set_param(int32_t state, int32_t output);
        int32_t item_state();
        int32_t item_output();
        table_item& operator=(const table_item& item);
};

class table {
    private:
        uint32_t row_count = 0;
        uint32_t column_count = 0;
        table_item **items;
    public:
        table();
        table(std::ifstream &state_file, std::ifstream &output_file);
        table_item value(uint32_t row, uint32_t column);
        uint32_t get_row_count();
        uint32_t get_column_count();
        void print();
        ~table();
};

class struct_synthesis {
    private:
        uint32_t d_tg_count = 0;
        uint32_t input_lines_count = 0;
        uint32_t output_lines_count = 0;
        uint32_t state_count = 0;
        std::vector<dnf> _dnf;
        std::vector<std::string> states;
        std::vector<uint32_t> outputs;
        table *_table = nullptr;
    public:
        struct_synthesis(table *synt_table);
        void sdnf();
        void mdnf();
        void print(std::ostream &);
        void check(std::ifstream &);
};

#endif
