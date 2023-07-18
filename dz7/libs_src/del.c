#include "operations.h"
double operation_div(double a, double b){
    return a / b;
}
struct Info_func_operation func_table[] = { {"operation_div", "div", 2} };
const int count_func = sizeof(func_table)/sizeof(struct Info_func_operation);