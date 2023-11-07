#include "interface.h"

table_item::table_item()
{
    this->state = this->output = 0;
}

void table_item::set_param(int32_t state, int32_t output)
{
    this->state = state;
    this->output = output;
} 

table_item::table_item(int32_t state, int32_t output)
{
    this->state = state;
    this->output = output;
}

table_item& table_item::operator=(const table_item& item)
{
    this->state = item.state;
    this->output = item.output;

    return *this;
}

int32_t table_item::item_state()
{
    return this->state;
}

int32_t table_item::item_output()
{
    return this->output;
}

table_item::~table_item()
{

}
