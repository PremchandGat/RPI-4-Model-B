/* Import Modules */
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/export.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
/* ------------------------------------------------------------------ */

#define DEV_MEM_SIZE 512

/* Global Vars */
/* dev_t type is the type of  uint32_t 
 * 12 bytes stores Major Number
 * 20 bytes stores Minor Number 
 * dev_t device_number;
 * int minor_no = MINOR(device_number)
 * int major_no = MAJOR(device_number)
 * MINOR and MAJOR are micros defined under linux/kdev_t.h */
dev_t device_number;
/* Device buffet to store data */
char pcd_buffer[DEV_MEM_SIZE];
/* Define vars struct cdev */
struct cdev pcd_cdev;
/* Define struct class pointer to store class */
struct class * pcd_class;
/* Define struct device pointer */
struct device * pcd_device;

/* ------------------------------------------------------------------ */



/* Open method implementation */
int pcd_open(struct inode *pnode, struct file *pfile){

	return 0;
}

/* Read method implementation 
 * pfile -> Pointer to file object
 * puser -> Pointer to user buffet
 * count -> Read count Given by user
 * f_pos -> Pointer to current file posistion from read has to begin 
 * Returns 0 if there is nothing to read 
 * Returns negative value if any error
 * Returns +ve value (Number of Sucessfully read bytes)
 * __user is macro which used with user level pointers which tells developer
 * not to trust or assume it as valid pointer to avoit kernel faults  
 * insted of directly using user level pointers use 
 * copy_to_user and copy_from_user kenel functions*/
ssize_t pcd_read(struct file *pfile, char __user *puser, size_t count, loff_t *f_pos ){
	pr_info("Read requested for %zu bytes\n",count);
	pr_info("Current file position = %lld\n",*f_pos);
	if((*f_pos + count) > DEV_MEM_SIZE){
		count = DEV_MEM_SIZE - *f_pos;
	}
	pr_info("Reading %zu bytes\n",count);
	/* unsigned long copy_to_user(void __user *to, const void *from, 
	 * 					unsigned long n);
	 * to -> Destination address in user space
	 * from -> source address in kernel space
	 * n -> number of bytes to copy
	 * Returns 0 if success 
	 * else returns number of bytes failed to copy */	
	if(copy_to_user(puser,&pcd_buffer[*f_pos], count)){
		pr_info("Failed to read");
		/* EFAULT is error code */
		return -EFAULT;
	}

	*f_pos += count;
	pr_info("Number of bytes successfully read = %zu\n",count);
	pr_info("Updated file position = %lld\n",*f_pos);
	return count;
}

/* Write method implementation 
 * count > write count given by user 
 * f_pos > pointer to current file position from where write has to begin */
ssize_t pcd_write(struct file *pfile, const char __user *puser,size_t count, loff_t *f_pos)
{
	pr_info("Write requested for %zu bytes\n",count);
	pr_info("Current file position = %lld\n",*f_pos);
	if((count + *f_pos) > DEV_MEM_SIZE){
		count = DEV_MEM_SIZE - *f_pos;
	}
	pr_info("Count value : %zu\n",count);
	if(!count){
		pr_err("No space left on the device \n");
		return -ENOMEM;
	}

	if(copy_from_user(&pcd_buffer[*f_pos],puser,count)){
		pr_err("Failed to copy data from user");
		return -EFAULT;
	}
	*f_pos += count;
	return count;

}
/* Seek method implementation 
 * pfile is pointer to file
 * whence can be
 * 		SEEK_SET The file offset is set to ‘off’ bytes
 * 		SEEK_CUR The file offset is set to its current location plus ‘off’ bytes.
 * 		SEEK_END The file offset is set to the size of the file plus ‘off’ bytes.
 * */
loff_t pcd_seek(struct file *pfile, loff_t f_pos, int whence){
	return pfile->f_pos;
}

/* Release method implementations 
 * pnode is pointer to inode 
 * pfile is pointer to file object*/
int pcd_release(struct inode *pnode,struct file *pfile){
	
	return 0;
}

struct file_operations pcd_fops = {
	.open =	pcd_open,
	.read = pcd_read,
	.write = pcd_write,
	.release = pcd_release,
	.llseek = pcd_seek,
	.owner = THIS_MODULE
};


/* This initialization function is called at the time of
 * Booting in case of static module
 * Module insetation in case of Dynamic module 
 * This functions duty is to intialize the Driver 
 *
 * This intialization function doesn't provides any servicec or 
 * functionalities to other modules so it's good practice to make
 * this module private by using static keyworkd 
 *
 * insmod <Module_Name> is the command used to insert the module into 
 * kernel */
static int __init pcd_driver_init(void){
	/* define int var to store return codes */
	int ret = 0;

	/* pr_info is used to print some information on console 
	 * There are multiple levels of Log messages like 
	 * KERN_EMERG -> pr_emerg 
	 * KERN_ALERT -> pr_alert
	 * KERN_CRIT  -> pr_crit 
	 * KERN_ERR -> pr_err  ......
	 * .......
	 * printk(KERN_ALERT"This is Alert Messaage");
	 * pr_alert("This is Alert Messge");
	 * Above both satements are same we can use any of them
	 * according to our convinienve
	 *
	 * PrintK is similar to the function printf 
	 * printf is user level but printk is kernel level
	 * */
	pr_info("Intializing driver.........\n");
	/* int alloc_chrdev_refion(dev_t *dev, unsigned baseminor, 
	 * 				unsigned count, const char *name);
	 * This function is defined under linux/fs.h 
	 * This function is used to register the device numbers */
	ret = alloc_chrdev_region(&device_number,0,1,"pcd_devices");
	if(ret < 0){
		pr_err("Failed alloc_chrdev_region\n");	
		goto out;
	}

	/* void cdev_init(struct cdev *cdev, 
	 * 			const struct file_operations *fops)
	 * This function used to initialize the cdev struct */
	cdev_init(&pcd_cdev,&pcd_fops);
	pcd_cdev.owner = THIS_MODULE;
	/* int cdev_add(struct cdev *p, dev_t dev, unsigned count);
	 * This function is used to add char device to the kernel VFS */
	ret = cdev_add(&pcd_cdev, device_number, 1);
	if(ret < 0){
		pr_err("Failed cdev_add\n");
		goto unregister_chrdev;
	}
	/* struct class * class_create(struct moduule *owner,
	 * 					const char *name) 
	 * THIS_MODULE is micro defined under linux/export.h  */
	pcd_class = class_create(THIS_MODULE,"pcd_class");
	if(IS_ERR(pcd_class)){
		pr_err("Failed class_create\n");
		ret = PTR_ERR(pcd_class);
		goto delete_cdev;
	}
	/* struct device *device_create(struct class *class, 
	 * 					struct device *parent, 
	 * 					dev_t devt, 
	 * 					void *drvdata,
	 * 					const *fmt,....) */
	pcd_device = device_create(pcd_class,NULL,device_number,NULL,"pcd");
	if(IS_ERR(pcd_device)){
		pr_err("Failed device_create\n");
		ret = PTR_ERR(pcd_device);
		goto delete_class;
	}
	/* Succesfully initialized driver so returnin 0 */
	pr_info("Module successfully loaded\n");
	return 0;
delete_class:
	class_destroy(pcd_class);
delete_cdev:
	cdev_del(&pcd_cdev);
unregister_chrdev:
	unregister_chrdev_region(device_number,1);
out:
	pr_err("Failed to initialize pcd driver exited.\n");
	return ret;

}


/* This exit function is called when 
 * This module is removed from system
 * The duty of this function is to clean or undo the 
 * operations which are done in intialization fucntion
 *
 * If we are sure that this is static module or this would not
 * be removed the system in this case no need to implement this 
 * function
 *
 * rmmod <Module_Name> is command used to remove the module from kernel
 * When we call this rmmode function that time this function is called
 *
 * In case of static module the kernel will ingore this function at the 
 * time of build process if we have provided the __exit marker */
static void __exit pcd_driver_exit(void){
	pr_info("Exiting from driver........\n");
	/* We need to reverse of what we have done in pcd_driver_int */
	/* Remove device that was created by device_create() */
	device_destroy(pcd_class,device_number);
	/* Destroy struct class structure */
	class_destroy(pcd_class);
	/* Remove cdev registration from the kernel VFS */
	cdev_del(&pcd_cdev);
	/* Unregister a range of device numbers */
	unregister_chrdev_region(device_number,1);
	pr_info("Exited\n");
}


/* Register initialization fucntion By using
 * module_init() Macro defined in linux/module.h*/
module_init(pcd_driver_init);

/* Register Exit function 
 * module_exit() Macro defined in linux/module.h*/
module_exit(pcd_driver_exit);



/* Provide details about The module 
 * modinfo <Module_Name>.ko is used to view Module Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PremchandGat");
MODULE_DESCRIPTION("This is PCD driver implemented for learning purpose");
MODULE_INFO(board,"Raspberry Pi 4 Model B");





