#include <math.h>
#include "operations.h"
double operation_cos(double a){
    return cos(a);
}
double operation_return_PI(){
    return M_PI;
}
struct Info_func_operation func_table[] = { {"operation_cos", "my_cos", 1}, {"operation_return_PI", "Pi", 0} };
const int count_func = sizeof(func_table)/sizeof(struct Info_func_operation);


