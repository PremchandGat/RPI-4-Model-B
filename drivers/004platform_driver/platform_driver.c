#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include <linux/platform_device.h>
#include<linux/slab.h>
#include<linux/mod_devicetable.h>
#include "device_data.h"

#define MAX_DEVICE_SUPPORT 5

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__
/*  insmod platform_driver.ko
    insmod plarform_device.ko
    rmmod platform_driver
    rmmod platform_device */

struct pcdev_private_data {
    char *buffer;
    dev_t device_number;
    struct cdev cdev;
    struct device_data dev_data;
};

struct pcdrv_private_data {
    int total_devices;
    dev_t device_number;
    struct class *class;
    struct device *device;      
};

struct pcdrv_private_data driver_data = {
    .total_devices = 0};

loff_t mseek(struct file *filep, loff_t off, int whence)
{
    unsigned  max = (int)((struct device_data)(((struct pcdev_private_data *)filep->private_data)->dev_data)).size;
    unsigned  temp = 0;
    pr_info("Seeking to: %lld",off);
    switch (whence)
    {
    case SEEK_CUR:
        temp = off + filep->f_pos;
        if(temp >= max || temp < 0)
            return -EINVAL;
        filep->f_pos = temp;
        break;
    case SEEK_SET:
        if(off >= max || off < 0)
            return -EINVAL;
        filep->f_pos = off;
        break;
    case SEEK_END:
        temp = off + filep->f_pos;
        if(temp >= max || temp < 0)
            return -EINVAL;
        filep->f_pos = temp;
        break;
    default:
        return -EINVAL;
        break;
    }
    pr_info("New seeked file position: %lld",filep->f_pos);
    return filep->f_pos;
};

ssize_t mread (struct file *filep, char __user *buff, size_t count, loff_t *f_pos){
   struct pcdev_private_data *dev = (struct pcdev_private_data *)filep->private_data;
    pr_info("Read requested for %zu bytes\n",count);
	pr_info("Current file position = %lld\n",*f_pos);
	if((*f_pos + count) > dev->dev_data.size){
		count = dev->dev_data.size - *f_pos;
	}
	pr_info("Reading %zu bytes\n",countGPIO line named );
	/* unsigned long copy_to_user(void __user *to, const void *from, 
	 * 					unsigned long n);
	 * to -> Destination address in user space
	 * from -> source address in kernel space
	 * n -> number of bytes to copy
	 * Returns 0 if success 
	 * else returns number of bytes failed to copy */	
	if(copy_to_user(buff,&dev->buffer[*f_pos], count)){
		pr_info("Failed to read");
		/* EFAULT is error code */
		return -EFAULT;
	}

	*f_pos += count;
	pr_info("Number of bytes successfully read = %zu\n",count);
	pr_info("Updated file position = %lld\n",*f_pos);
 return count;
}
ssize_t mwrite (struct file *filep, const char __user *buff, size_t count, loff_t *f_pos){
    struct pcdev_private_data *dev = (struct pcdev_private_data *)filep->private_data;

	pr_info("Write requested for %zu bytes\n",count);
	pr_info("Current file position = %lld\n",*f_pos);

    if ((count + *f_pos) > dev->dev_data.size)
    {
        count = dev->dev_data.size - *f_pos;
    }
    pr_info("Count value : %zu\n",count);
	if(!count){
		pr_err("No space left on the device \n");
		return -ENOMEM;
	}

	if(copy_from_user(&dev->buffer[*f_pos],buff,count)){
		pr_err("Failed to copy data from user");
		return -EFAULT;
	}
	*f_pos += count;
	return count;
}

int validatePermission(int dev_perm , int acc_perm){
    if(dev_perm == RDWR)
    {   
        return 0;
    }

    if(dev_perm == RDONLY && ((acc_perm & FMODE_READ) && !(acc_perm & FMODE_WRITE)))
    {
        return 0;
    }
    
    if(dev_perm == WRONLY && ((acc_perm & FMODE_WRITE) && !(acc_perm & FMODE_READ)))
    {
        return 0;
    }
    
    return -EPERM;
}


int mopen (struct inode *nodep, struct file *filep){
    int ret = 0;
    struct pcdev_private_data *dev_data;
    pr_info("Opening File minor : %d", MINOR(nodep->i_rdev));
    dev_data = container_of(nodep->i_cdev, struct pcdev_private_data, cdev);
    filep->private_data = dev_data;
    ret = validatePermission(dev_data->dev_data.perm, filep->f_mode);
    if(ret) {
        pr_err("Failed to open No permission");
    }else{
        pr_info("Open Success to file");
    }
    return ret;
    return 0;
}

int mrelease (struct inode *nodep, struct file *filep){
    pr_info("Release success!");
    return 0;
}

struct file_operations mfops = {
    .owner = THIS_MODULE,
    .llseek = mseek,
    .read = mread,
    .write = mwrite,
    .open = mopen,
    .release = mrelease,
};


int probe(struct platform_device *devicep) {
    int err = 0;
    struct pcdev_private_data * devp_data;
    struct device_data * dev_d = (struct device_data *)devicep->dev.platform_data;
    pr_info("Device detected!\n");
    /* Allocate some memory*/
    devp_data = devm_kzalloc(&devicep->dev, sizeof(* devp_data), GFP_KERNEL);
    if(!devp_data){
        return -ENOMEM;
    }
    devicep->dev.driver_data = (void*)devp_data;
    devp_data->dev_data.size = dev_d->size;
    devp_data->dev_data.serial_name = dev_d->serial_name;
    devp_data->dev_data.perm = dev_d->perm;
    devp_data->device_number = driver_data.device_number + driver_data.total_devices + 1;
    pr_info("Device size      : %d\n", dev_d->size);
    pr_info("Device permission: %d\n",dev_d->perm);
    pr_info("Device name      : %s\n",dev_d->serial_name);

    devp_data->buffer = devm_kzalloc(&devicep->dev, devp_data->dev_data.size, GFP_KERNEL);
    if(!devp_data->buffer){
        return -ENOMEM;
    }

    cdev_init(&devp_data->cdev, &mfops);
    devp_data->cdev.owner = THIS_MODULE;
    err = cdev_add(&devp_data->cdev,devp_data->device_number,1);
    if(err < 0){
            goto out;
    }

    driver_data.device = device_create(driver_data.class, NULL, devp_data->device_number, NULL, "pdriver-%d", driver_data.total_devices + 1);
    if(IS_ERR(driver_data.device)){
           pr_err("Failed to add device\n");
           err = PTR_ERR(driver_data.device);
           goto destroy_cdev;
        }
        pr_info("Device added\n");
        driver_data.total_devices++;
        return 0;

destroy_cdev:
    cdev_del(&devp_data->cdev);

out:
    pr_err("Failed to add driver");
    return err;
};

int remove(struct platform_device *devicep){
    struct pcdev_private_data * devp_data;
    devp_data = (struct pcdev_private_data *)devicep->dev.driver_data;
    pr_info("Removing Device\n");
    cdev_del(&devp_data->cdev);
    device_destroy(driver_data.class, devp_data->device_number);
    driver_data.total_devices--;
    return 0;
};


struct platform_device_id device_ids_table[] = 
{
	[0] = {.name = "pcd-a1", .driver_data = 1 },
	{ } /*Null termination */
};

struct platform_driver pcd_driver = {
    .probe = probe,
    .remove = remove,
    .id_table = device_ids_table,
    .driver = {
        .name = "pcd-a1"
    }
};

static int __init pcd_driver_init(void){
    int ret = 0;
    pr_info("Intializing driver\n");
    ret =  platform_driver_register(&pcd_driver);
    if(ret < 0) goto out;

    ret = alloc_chrdev_region(&driver_data.device_number,0,MAX_DEVICE_SUPPORT,"platform_driver");
    if(ret < 0 ) goto unregister_driver; 
    
    driver_data.class = class_create(THIS_MODULE,"platforn_driver");
    if(IS_ERR(driver_data.class)){
        ret = PTR_ERR(driver_data.class);
        goto unregister_chrdev;
    }
    pr_info("Driver initialized\n");
    return 0;

unregister_chrdev:
	unregister_chrdev_region(driver_data.device_number,MAX_DEVICE_SUPPORT);

unregister_driver:
    platform_driver_unregister(&pcd_driver);

out:
	pr_err("Failed to initialize pcd driver exited.\n");
    return ret;
}

static void __exit pcd_driver_exit(void){
    platform_driver_unregister(&pcd_driver);
    class_destroy(driver_data.class);
    unregister_chrdev_region(driver_data.device_number,MAX_DEVICE_SUPPORT);
    pr_info("Exited driver\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_exit);
MODULE_AUTHOR("PremchandGat");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PCD Multi Platform driver");
