#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/export.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#define NO_OF_DEVICES 4

#define DEV_SIZE0 512
#define DEV_SIZE1 512
#define DEV_SIZE2 1024
#define DEV_SIZE3 1024

/*permission codes */
#define RDONLY 0x01
#define WRONLY 0X10
#define RDWR   0x11

char mbuff0[DEV_SIZE0];
char mbuff1[DEV_SIZE1];
char mbuff2[DEV_SIZE2];
char mbuff3[DEV_SIZE3];

struct pcdev_private_data {
    char *buffer;
    unsigned size;
    const char *serial_number;
    int perm;
    struct cdev cdev;
};

struct pcdrv_private_data {
    int total_devices;
    dev_t device_number;
    struct class *class;
    struct device *device;
    struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};


struct pcdrv_private_data driver = {
    .total_devices = NO_OF_DEVICES,
    .pcdev_data = {
        [0] = {
            .buffer = mbuff0,
            .size = DEV_SIZE0,
            .serial_number = "mdev-1",
            .perm  = RDONLY
        },
        [1] = {
            .buffer = mbuff1,
            .size = DEV_SIZE1,
            .serial_number = "mdev-2",
            .perm  = WRONLY
        },
        [2] = {
            .buffer = mbuff2,
            .size = DEV_SIZE2,
            .serial_number = "mdev-3",
            .perm  = RDWR
        },
        [3] = {
            .buffer = mbuff3,
            .size = DEV_SIZE3,
            .serial_number = "mdev-4",
            .perm  = RDONLY
        }
    }
};

loff_t mseek(struct file *filep, loff_t off, int whence){
    unsigned  max = (unsigned)((struct pcdev_private_data *)filep->private_data)->size;
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
	if((*f_pos + count) > dev->size){
		count = dev->size - *f_pos;
	}
	pr_info("Reading %zu bytes\n",count);
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

    if ((count + *f_pos) > dev->size)
    {
        count = dev->size - *f_pos;
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
    ret = validatePermission(dev_data->perm, filep->f_mode );
    if(ret) {
        pr_err("Failed to open No permission");
    }else{
        pr_info("Open Success to file");
    }
    return ret;
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




static int __init pcd_multi_init(void){
    int err = 0;
    int i= 0;
    pr_info("Inserting PCDM module");
    err = alloc_chrdev_region(&driver.device_number,0,4,"mdev");
    if(err < 0 ) goto out; 
    driver.class = class_create(THIS_MODULE,"mdev");
    if(IS_ERR(driver.class)){
        err = PTR_ERR(driver.class);
        goto unregister_chrdev;
    }
    for(i = 0; i < NO_OF_DEVICES; i ++ ){
        cdev_init(&driver.pcdev_data[i].cdev, &mfops);
        driver.pcdev_data[i].cdev.owner = THIS_MODULE;
        err = cdev_add(&driver.pcdev_data[i].cdev, driver.device_number+i,1);
        if(err < 0){
            goto destroy;
        } 
        driver.device = device_create(driver.class,NULL,driver.device_number+i, NULL , driver.pcdev_data[i].serial_number);
        if(IS_ERR(driver.device)){
            pr_err("Failed to create device");
           err = PTR_ERR(driver.device);
           goto destroy;
        }
    }
    return 0;

destroy:
	for(; i >= 0; i--){
        device_destroy(driver.class, driver.device_number+i);
		cdev_del(&driver.pcdev_data[i].cdev);
	}
    class_destroy(driver.class);
unregister_chrdev:
	unregister_chrdev_region(driver.device_number,NO_OF_DEVICES);
out:
	pr_err("Failed to initialize pcd driver exited.\n");
	return err;  
}

static void __exit pcd_multi_exit(void){
    pr_info("Exiting Milti pcd Driver");
	for(int i = NO_OF_DEVICES - 1; i >= 0; i--){
        device_destroy(driver.class, driver.device_number+i);
		cdev_del(&driver.pcdev_data[i].cdev);
	}
    class_destroy(driver.class);
	unregister_chrdev_region(driver.device_number,NO_OF_DEVICES);
	pr_info("Exited The multi PCD driver\n");  
}

module_init(pcd_multi_init);
module_exit(pcd_multi_exit);
MODULE_AUTHOR("PremchandGat");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PCD Multi driver");


