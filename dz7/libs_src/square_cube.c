#include "operations.h"
double operation_cube(double a){
    return a * a * a;
}
double operation_square(double a){
    return a * a;
}
struct Info_func_operation func_table[] = { {"operation_square", "square", 1}, {"operation_cube", "cube", 1} };
const int count_func = sizeof(func_table)/sizeof(struct Info_func_operation);