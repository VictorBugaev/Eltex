#include "operations.h"
double operation_add(double a, double b){
    return a + b;
}
double operation_add2(double a, double b, double c){
    return a + b + c;
}

const struct Info_func_operation func_table[] = { {"operation_add", "add", 2}, {"operation_add2", "add2", 3} };
const int count_func = sizeof(func_table)/sizeof(struct Info_func_operation);