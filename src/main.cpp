#include <iostream>
#include <fstream>
#include "interface.h"

#define I_OUTPUT_FILE "g_table.txt"
#define I_STATE_FILE  "f_table.txt"
#define O_OUTPUT_FILE "output.txt"
#define I_CHECK_FILE  "check.txt"

int main()
{
    /* Открытие файлов с таблицей переходов и таблицей выходов */
    std::ifstream state_file(I_STATE_FILE);
    std::ifstream output_file(I_OUTPUT_FILE);

    table my_table(state_file, output_file);

    struct_synthesis synt(&my_table);

    synt.sdnf();
    synt.mdnf();

    std::ofstream out(O_OUTPUT_FILE);

    synt.print(out);

    state_file.close();
    output_file.close();
    out.close();

    std::ifstream check_file(I_CHECK_FILE);

    synt.check(check_file);

    return 0;
}
