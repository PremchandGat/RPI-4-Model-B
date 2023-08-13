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
#define DRIVER_NAME "prem_custom_serial"
#define DELAY_TIME		(HZ * 2)	/* 2 seconds per character */

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PremchandGat");
MODULE_DESCRIPTION("Custom TTY driver for serial communication");

struct prem_tty_driver {
    struct timer_list ttimer;
    struct tty_driver *custom_tty;
    struct device *custom_device;
    struct tty_port tt_port;
    struct tty_struct tty;
};



void read_timer(struct timer_list *t){
     pr_info("Reading data\n");
     struct prem_tty_driver *tiny = from_timer(tiny, t, ttimer);
     if(!tiny){
         pr_err("Failed to get data inside timer callback");
         return;
     }
     pr_info("Reading data after tiny\n");
     char data[1] = {'x'};
     struct tty_port *port = &tiny->tt_port;
    for (int i = 0; i < 1; ++i) {
		// if (!tty_buffer_request_room(port, 1)){
        //     pr_info("Push tty_flip_buffer_push\n");
        //     tty_flip_buffer_push(port);
        // }
		tty_insert_flip_char(port, data[i], TTY_NORMAL);
	}
	// tty_flip_buffer_push(port);
	tiny->ttimer.expires = jiffies + DELAY_TIME;
	add_timer(&tiny->ttimer);
    pr_info("Read done\n");
}

int prem_tty_open(struct tty_struct * ttyp, struct file * filp){
    struct prem_tty_driver *drv;
    pr_info("opening the tty....\n");
    drv = container_of(ttyp, struct prem_tty_driver, tty);
    timer_setup(&drv->ttimer, read_timer, 0);
	drv->ttimer.expires = jiffies + DELAY_TIME;
	add_timer(&drv->ttimer);
    // mod_timer(&drv->ttimer, jiffies + msecs_to_jiffies(1000));
    pr_info("opening the done\n");
    return 0;
}

int  prem_tty_write(struct tty_struct * tty,
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
        struct prem_tty_driver *drv =  container_of(tty, struct prem_tty_driver, tty);
        pr_info("closing the tty....\n");
        del_timer(&drv->ttimer);
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


struct prem_tty_driver prem_driver;
// struct tty_port_opearations tt_ops = {};



static int __init _prem_serial_init(void)
{
    int err = 0;
    prem_driver.custom_tty = __tty_alloc_driver(1, THIS_MODULE, TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV);
    
    pr_info("Initializing the tty driver\n");
    if(IS_ERR(prem_driver.custom_tty)){
        pr_err("Failed to alloc driver\n");
        err = PTR_ERR(prem_driver.custom_tty);
        goto unalloc_driver;
    }
    prem_driver.custom_tty->owner = THIS_MODULE;
    prem_driver.custom_tty->driver_name = "prem_tty_driver";
    prem_driver.custom_tty->name = "premttt";
    prem_driver.custom_tty->type = TTY_DRIVER_TYPE_SERIAL;
    prem_driver.custom_tty->subtype = SERIAL_TYPE_NORMAL;
    prem_driver.custom_tty->init_termios = tty_std_termios;
    prem_driver.custom_tty->num = 1;
    prem_driver.custom_tty->ops = &tty_ops;
	prem_driver.custom_tty->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
    tty_set_operations(prem_driver.custom_tty, &tty_ops);
    err = tty_register_driver(prem_driver.custom_tty);
    if(err < 0){
        pr_err("Failed to register driver\n");
        return err;
    }
    pr_info("Driver registerd\n");
    tty_port_init(&prem_driver.tt_port);
    pr_info("Registering device\n");
    prem_driver.custom_device = tty_port_register_device(&prem_driver.tt_port, prem_driver.custom_tty, 0, NULL);
    if (IS_ERR(prem_driver.custom_device))
    {
        pr_err("Failed to register port and device\n");
        err = PTR_ERR(prem_driver.custom_device);
        goto unregister_driver;
    }
    pr_info("Custom tty driver intialized\n");
    return 0;

unregister_driver:
    tty_port_destroy(&prem_driver.tt_port);
    tty_unregister_driver(prem_driver.custom_tty);
unalloc_driver:
    tty_driver_kref_put(prem_driver.custom_tty);
    return err;
}

static void __exit _prem_serial_exit(void) {
    tty_unregister_device(prem_driver.custom_tty,0);
    tty_unregister_driver(prem_driver.custom_tty);
    tty_driver_kref_put(prem_driver.custom_tty);
    tty_port_destroy(&prem_driver.tt_port);
    pr_info("Exited TTY driver\n");
}

module_init(_prem_serial_init);
module_exit(_prem_serial_exit);
