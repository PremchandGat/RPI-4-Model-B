#include <linux/module.h>
#include<linux/platform_device.h>
#include "device_data.h"


#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

void release(struct device *dev){
    pr_info("Device released!!\n");
};

struct device_data devices[] = {
    [0] = {.perm = RDWR, .size = 512, .serial_name = "pcdev-A"},
    [1] = {.perm = RDWR, .size = 1024, .serial_name = "pcdev-B"}
};

struct platform_device platform_device1 = {
    .name = "pcd-a1", 
    .id = 0, 
    .dev = {
        .platform_data = &devices[0],
        .release = release,
    }
};

struct platform_device platform_device2 = {
    .name = "pcd-a1",
    .id = 1, 
    .dev = {
        .platform_data = &devices[1],
        .release = release,
    }
};

static int __init pcd_device_init(void) {
    int err = 0;
    pr_info("Registering devices\n");
    err = platform_device_register(&platform_device1);
    if(err < 0 )
    {
        goto out;
    }
    err = platform_device_register(&platform_device2);
    if(err < 0)
    {  
        goto unregister1;
    }
    return 0;
    
unregister1:
    platform_device_unregister(&platform_device1);
out:
    pr_err("Failed to register Platform device\n");
    return err;
}

static void __exit pcd_device_exit(void){
    platform_device_unregister(&platform_device1);
    platform_device_unregister(&platform_device2);
    pr_info("Exiting pcd platform device\n");
}


module_init(pcd_device_init);
module_exit(pcd_device_exit);

MODULE_AUTHOR("PremchandGat");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PCD Multi Platform devices satup");
