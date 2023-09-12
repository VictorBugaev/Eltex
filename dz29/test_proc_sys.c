#include <linux/kernel.h>
#include <linux/module.h> 
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/rwlock_types.h>

#define SIZE_ARRAY 100
#define PROCFILE "test_proc"
#define SYSFILE "test_sys"


static ssize_t proc_readfile(struct file *fd, char __user *buffer, size_t len, loff_t *off);
static ssize_t proc_writefile(struct file *fd, const char __user *buffer, size_t len, loff_t *off);

static ssize_t sys_showfile(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sys_storefile(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count);

static struct proc_dir_entry *our_proc_file;
static char array[SIZE_ARRAY] = "TEST MODULE";
static rwlock_t lock;


static struct proc_ops proc_fops = {
    .proc_read = proc_readfile,
    .proc_write = proc_writefile,
};

static struct kobject *mymodule;

static struct kobj_attribute attribute_arr = __ATTR(array, 0664, sys_showfile, (void *)sys_storefile);

static struct attribute *attrs[] = {
    &attribute_arr.attr,
    NULL
};
static struct attribute_group attr_group = {
    .attrs = attrs
};

int init_module(void){
    rwlock_init(&lock);

    our_proc_file = proc_create(PROCFILE, 0644, NULL, &proc_fops);
    if (NULL == our_proc_file) {
        proc_remove(our_proc_file);
        pr_alert("Error initialize /proc/%s\n", PROCFILE);
        return -ENOMEM;
    }

    mymodule = kobject_create_and_add(SYSFILE, kernel_kobj);
    if(!mymodule){
        proc_remove(our_proc_file);
        return -ENOMEM;
    }

    int error = sysfs_create_file(mymodule, &attribute_arr.attr);
    if (error) {
        proc_remove(our_proc_file);
        kobject_put(mymodule);
        pr_info("Error create /sys/kernel/%s\n", PROCFILE);
        return error;
    }

    pr_info("Load module\n");
    return 0;
}

void cleanup_module(void){
    proc_remove(our_proc_file);
    kobject_put(mymodule);
    pr_info("Unload module\n");
}



static ssize_t proc_readfile(struct file *fd, char __user *buffer, size_t len, loff_t *off){
    int res;
    res = simple_read_from_buffer(buffer, len, off, array, SIZE_ARRAY);
    return res;
}

static ssize_t proc_writefile(struct file *fd, const char __user *buffer, size_t len, loff_t *off){
    ssize_t res;
    if(len > SIZE_ARRAY){
        pr_alert("test_proc: error len - %d\n", (int)len);
        return -EINVAL;
    }
    write_lock(&lock);
    res = simple_write_to_buffer(array, SIZE_ARRAY, off, buffer, len);
    write_unlock(&lock);
    return res;
}

static ssize_t sys_showfile(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    memcpy(buf, array, SIZE_ARRAY);
    return SIZE_ARRAY;
}
static ssize_t sys_storefile(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count){
    if(count > SIZE_ARRAY){
        pr_info("Error: size array = 100 < size buf = %d\n", (int)count);
        memcpy(array, buf, SIZE_ARRAY);
        return SIZE_ARRAY;
    }
    else
        memcpy(array, buf, count);
    return count;
}


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Andrey");
MODULE_DESCRIPTION("Test proc, sys");

