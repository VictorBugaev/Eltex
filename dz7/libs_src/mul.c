#include "operations.h"
double operation_mul(double a, double b){
    return a * b;
}
struct Info_func_operation func_table[] = { {"operation_mul", "mul", 2} };
const int count_func = sizeof(func_table)/sizeof(struct Info_func_operation);