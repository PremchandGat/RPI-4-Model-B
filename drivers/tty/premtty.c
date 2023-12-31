#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/tty_flags.h>

#define DRIVER_NAME "prem_custom_serial"
#define DELAY_TIME		(HZ * 2)	/* 2 seconds per character */
#define TINY_TTY_MINORS 1
#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PremchandGat");
MODULE_DESCRIPTION("Custom TTY driver for serial communication");

struct tty_prv_data {
    struct tty_struct	*tty;		/* pointer to the tty for this device */
	struct timer_list	timer;
};

struct driver_data {
    struct tty_driver *driver;
    struct tty_port port;
};

struct driver_data p_driver;
struct device *p_device;
static struct tty_port_operations port_ops = { 0 };

void read_timer(struct timer_list *t){
     pr_info("Reading data\n");
     struct tty_prv_data *tiny = from_timer(tiny, t, timer);
     struct tty_struct *tty;
     struct tty_port *port;
     if (!tiny)
     {
         pr_err("Failed to get data inside timer callback\n");
         return;
     }
     pr_info("Reading data after tiny\n");
     char data[1] = {'p'};
     tty = tiny->tty;
     port = tty->port;
     if(!port){
         pr_err("We got NULL port here returning\n");
         return;
     }
    for (int i = 0; i < 1; ++i) {
		if (!tty_buffer_request_room(port, 1)){
            pr_info("Push tty_flip_buffer_push\n");
            tty_flip_buffer_push(port);
        }
		tty_insert_flip_char(port, data[0], TTY_NORMAL);
	 }
	tty_flip_buffer_push(port);
	tiny->timer.expires = jiffies + DELAY_TIME;
	add_timer(&tiny->timer);
    pr_info("Read done\n");
}


int prem_tty_open(struct tty_struct * ttyp, struct file * filp) {
    struct tty_prv_data *dev_data;
    pr_info("opening the tty....\n");

    ttyp->flags = ASYNC_LOW_LATENCY;
    /* first time accessing this device, let's create it */
    dev_data = kmalloc(sizeof(*dev_data), GFP_KERNEL);
	if (!dev_data)
		return -ENOMEM;


	/* save our structure within the tty structure */
	ttyp->driver_data = dev_data;
	dev_data->tty = ttyp;

    timer_setup(&dev_data->timer, read_timer, 0);
	dev_data->timer.expires = jiffies + DELAY_TIME;
	add_timer(&dev_data->timer);
    // mod_timer(&drv->ttimer, jiffies + msecs_to_jiffies(1000));
    pr_info("opening done\n");
    return 0;
    return tty_port_open(ttyp->port, ttyp, filp);
}


int prem_tty_write(struct tty_struct * tty,
		      const unsigned char *buf, int count){
                  pr_info("writing the tty....\n");
                  for (size_t i = 0; i < count; i++)
                  {
                    pr_info("%02x ", buf[i]);
                  }
                  
                  pr_info("Writing: %s", buf);
                  return 0;
}


void prem_tty_close(struct tty_struct * tty, struct file * filp){
        struct tty_prv_data *drv =  tty->driver_data;
        pr_info("closing the tty....\n");
        del_timer(&drv->timer);
        kfree(drv);
}

unsigned int prem_tty_write_room(struct tty_struct *tty){
      return 255;
}


struct tty_operations tty_ops  = {
    .open = prem_tty_open,
    .write = prem_tty_write,
    .close = prem_tty_close,
    .write_room = prem_tty_write_room
};


static int __init _prem_serial_init(void)
{
    int err = 0;
    p_driver.driver = __tty_alloc_driver(1, THIS_MODULE, TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV);
    
    pr_info("Initializing the tty driver\n");
    if(IS_ERR(p_driver.driver)){
        pr_err("Failed to alloc driver\n");
        err = PTR_ERR(p_driver.driver);
        goto unalloc_driver;
    }

    p_driver.driver->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV,
    p_driver.driver->owner = THIS_MODULE;
    p_driver.driver->driver_name = "prem_tty_driver";
    p_driver.driver->name = "premttt";
    p_driver.driver->type = TTY_DRIVER_TYPE_SERIAL;
    p_driver.driver->subtype = SERIAL_TYPE_NORMAL;
    p_driver.driver->init_termios = tty_std_termios;
    p_driver.driver->num = 1;
    p_driver.driver->ops = &tty_ops;
    p_driver.driver->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
    
    tty_set_operations(p_driver.driver, &tty_ops);
    
    err = tty_register_driver(p_driver.driver);
    
    if(err < 0){
        pr_err("Failed to register driver\n");
        return err;
    }
    pr_info("Driver registerd\n");
    tty_port_init(&p_driver.port);
    p_driver.port.ops = &port_ops;
	// tty_port_link_device(&prem_driver.tt_port, p_driver.driver, 0);
    pr_info("Registering device\n");
    p_device = tty_port_register_device(&p_driver.port, p_driver.driver, 0, NULL);
    if (IS_ERR(p_device))
    {
        pr_err("Failed to register port and device\n");
        err = PTR_ERR(p_device);
        goto unregister_driver;
    }
    pr_info("Custom tty driver intialized\n");
    return 0;

unregister_driver:
    tty_port_destroy(&p_driver.port);
    tty_unregister_driver(p_driver.driver);
unalloc_driver:
    tty_driver_kref_put(p_driver.driver);
    return err;
}

static void __exit _prem_serial_exit(void) {
    tty_unregister_device(p_driver.driver,0);
    tty_unregister_driver(p_driver.driver);
    tty_driver_kref_put(p_driver.driver);
    tty_port_destroy(&p_driver.port);
    pr_info("Exited TTY driver\n");
}

module_init(_prem_serial_init);
module_exit(_prem_serial_exit);

