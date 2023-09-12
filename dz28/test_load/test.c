#include <linux/kernel.h>
#include <linux/module.h> 




int init_module(void){

    pr_info("Load module test\n");

    return 0;
}
void cleanup_module(void){

    pr_info("Unload module test\n");
    
}

MODULE_LICENSE("GPL");



