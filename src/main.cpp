#include <iostream>
#include <fstream>
#include "interface.h"

#define I_OUTPUT_FILE "g_table.txt"
#define I_STATE_FILE  "f_table.txt"
#define O_OUTPUT_FILE "output.txt"

int main()
{
    /* Открытие файлов с таблицей переходов и таблицей выходов */
    std::ifstream state_file(I_STATE_FILE);
    std::ifstream output_file(I_OUTPUT_FILE);

    table my_table(state_file, output_file);

    my_table.print();
    struct_synthesis synt(&my_table);

    return 0;
}
