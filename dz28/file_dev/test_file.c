#include <linux/kernel.h>
#include <linux/module.h> 
#include <linux/fs.h>

#include <linux/moduleparam.h>
#include <linux/rwlock_types.h>

#define SIZE_ARRAY 100
#define DEVICE_NAME "test_file"
#define DEBUG

static ssize_t file_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t file_write(struct file *, const char __user *, size_t, loff_t *);

static char array[SIZE_ARRAY] = "TEST MODULE";
static rwlock_t lock;
static int major;

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = file_read,
    .write = file_write,
};


int init_module(void){

    rwlock_init(&lock);
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_alert("[test_file] error reg device\n");
        return major;
    }
    pr_info("Load module test_file, major number = %d\n", major);
    return 0;
}
void cleanup_module(void){
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Unload module test_file\n");
}

static ssize_t file_read(struct file *fd, char __user *buffer, size_t len, loff_t *off){
    int res;
    res = simple_read_from_buffer(buffer, len, off, array, SIZE_ARRAY);
    return res;
}

static ssize_t file_write(struct file *fd, const char __user *buffer, size_t len, loff_t *off){
    ssize_t res;
    if(len > SIZE_ARRAY){
        pr_alert("test_file: error len - %d\n", (int)len);
        return -EINVAL;
    }
    write_lock(&lock);
    res = simple_write_to_buffer(array, SIZE_ARRAY, off, buffer, len);
    write_unlock(&lock);
    return res;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrey");
MODULE_DESCRIPTION("My module");


