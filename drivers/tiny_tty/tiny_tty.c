/*
 * Tiny TTY driver
 *
 * Copyright (C) 2002-2004 Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, version 2 of the License.
 *
 * This driver shows how to create a minimal tty driver.  It does not rely on
 * any backing hardware, but creates a timer that emulates data being received
 * from some kind of hardware.
 */

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

#define DRIVER_VERSION "v2.0"
#define DRIVER_AUTHOR "Greg Kroah-Hartman <greg@kroah.com>"
#define DRIVER_DESC "Tiny TTY driver"

/* Module information */
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

#define DELAY_TIME		(HZ * 2)	/* 2 seconds per character */
#define TINY_DATA_CHARACTER	't'

#define TINY_TTY_MAJOR		240	/* experimental range */
#define TINY_TTY_MINORS		4	/* only have 4 devices */

struct tiny_serial {
	struct tty_struct	*tty;		/* pointer to the tty for this device */
	int			open_count;	/* number of times this port has been opened */
	struct mutex	mutex;		/* locks this structure */
	struct timer_list	timer;

	/* for tiocmget and tiocmset functions */
	int			msr;		/* MSR shadow */
	int			mcr;		/* MCR shadow */

	/* for ioctl fun */
	struct serial_struct	serial;
	wait_queue_head_t	wait;
	struct async_icount	icount;
};

static struct tiny_serial *tiny_table[TINY_TTY_MINORS];	/* initially all NULL */
static struct tty_port tiny_tty_port[TINY_TTY_MINORS];


static void tiny_timer(struct timer_list *t)
{
	struct tiny_serial *tiny = from_timer(tiny, t, timer);
	struct tty_struct *tty;
	struct tty_port *port;
	int i;
	char data[1] = {TINY_DATA_CHARACTER};
	int data_size = 1;

	if (!tiny)
		return;

	tty = tiny->tty;
	port = tty->port;

	/* send the data to the tty layer for users to read.  This doesn't
	 * actually push the data through unless tty->low_latency is set */
	for (i = 0; i < data_size; ++i) {
		if (!tty_buffer_request_room(port, 1))
			tty_flip_buffer_push(port);
		tty_insert_flip_char(port, data[i], TTY_NORMAL);
	}
	tty_flip_buffer_push (port);

	/* resubmit the timer again */
	tiny->timer.expires = jiffies + DELAY_TIME;
	add_timer(&tiny->timer);
}

static int tiny_open(struct tty_struct *tty, struct file *file)
{
	struct tiny_serial *tiny;
	int index;

	/* initialize the pointer in case something fails */
	tty->driver_data = NULL;

	/* get the serial object associated with this tty pointer */
	index = tty->index;
	tiny = tiny_table[index];
	if (tiny == NULL) {
		/* first time accessing this device, let's create it */
		tiny = kmalloc(sizeof(*tiny), GFP_KERNEL);
		if (!tiny)
			return -ENOMEM;

		mutex_init(&tiny->mutex);
		tiny->open_count = 0;

		tiny_table[index] = tiny;
	}

	mutex_lock(&tiny->mutex);

	/* save our structure within the tty structure */
	tty->driver_data = tiny;
	tiny->tty = tty;

	++tiny->open_count;
	if (tiny->open_count == 1) {
		/* this is the first time this port is opened */
		/* do any hardware initialization needed here */

		/* create our timer and submit it */
		timer_setup(&tiny->timer, tiny_timer, 0);
		tiny->timer.expires = jiffies + DELAY_TIME;
		add_timer(&tiny->timer);
	}

	mutex_unlock(&tiny->mutex);
	return 0;
}

static void do_close(struct tiny_serial *tiny)
{
	mutex_lock(&tiny->mutex);

	if (!tiny->open_count) {
		/* port was never opened */
		goto exit;
	}

	--tiny->open_count;
	if (tiny->open_count <= 0) {
		/* The port is being closed by the last user. */
		/* Do any hardware specific stuff here */

		/* shut down our timer */
		del_timer(&tiny->timer);
	}
exit:
	mutex_unlock(&tiny->mutex);
}

static void tiny_close(struct tty_struct *tty, struct file *file)
{
	struct tiny_serial *tiny = tty->driver_data;

	if (tiny)
		do_close(tiny);
}

static int tiny_write(struct tty_struct *tty,
		      const unsigned char *buffer, int count)
{
	struct tiny_serial *tiny = tty->driver_data;
	int i;
	int retval = -EINVAL;

	if (!tiny)
		return -ENODEV;

	mutex_lock(&tiny->mutex);

	if (!tiny->open_count)
		/* port was not opened */
		goto exit;

	/* fake sending the data out a hardware port by
	 * writing it to the kernel debug log.
	 */
	pr_debug("%s - ", __func__);
	for (i = 0; i < count; ++i)
		pr_info("%02x ", buffer[i]);
	pr_info("\n");

exit:
	mutex_unlock(&tiny->mutex);
	return retval;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 14, 0)) 
static int tiny_write_room(struct tty_struct *tty)
#else
static unsigned int tiny_write_room(struct tty_struct *tty)
#endif
{
	struct tiny_serial *tiny = tty->driver_data;
	int room = -EINVAL;

	if (!tiny)
		return -ENODEV;

	mutex_lock(&tiny->mutex);

	if (!tiny->open_count) {
		/* port was not opened */
		goto exit;
	}

	/* calculate how much room is left in the device */
	room = 255;

exit:
	mutex_unlock(&tiny->mutex);
	return room;
}

#define RELEVANT_IFLAG(iflag) ((iflag) & (IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK))


/* Our fake UART values */
#define MCR_DTR		0x01
#define MCR_RTS		0x02
#define MCR_LOOP	0x04
#define MSR_CTS		0x08
#define MSR_CD		0x10
#define MSR_RI		0x20
#define MSR_DSR		0x40


static const struct tty_operations serial_ops = {
	.open = tiny_open,
	.close = tiny_close,
	.write = tiny_write,
	.write_room = tiny_write_room,
	// .set_termios = tiny_set_termios,
	// .proc_show = tiny_proc_show,
	// .tiocmget = tiny_tiocmget,
	// .tiocmset = tiny_tiocmset,
	// .ioctl = tiny_ioctl,
};

static struct tty_driver *tiny_tty_driver;

static int __init tiny_init(void)
{
	int retval;
	int i;

	/* allocate the tty driver */
	// tiny_tty_driver = alloc_tty_driver(TINY_TTY_MINORS);
    tiny_tty_driver = __tty_alloc_driver(4, THIS_MODULE, TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV);
    
	if (!tiny_tty_driver)
		return -ENOMEM;

	/* initialize the tty driver */
	tiny_tty_driver->owner = THIS_MODULE;
	tiny_tty_driver->driver_name = "tiny_tty";
	tiny_tty_driver->name = "ttty";
	tiny_tty_driver->major = TINY_TTY_MAJOR,
	tiny_tty_driver->type = TTY_DRIVER_TYPE_SERIAL,
	tiny_tty_driver->subtype = SERIAL_TYPE_NORMAL,
	tiny_tty_driver->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV,
	tiny_tty_driver->init_termios = tty_std_termios;
	tiny_tty_driver->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	tty_set_operations(tiny_tty_driver, &serial_ops);
	for (i = 0; i < TINY_TTY_MINORS; i++) {
		tty_port_init(tiny_tty_port + i);
		tty_port_link_device(tiny_tty_port + i, tiny_tty_driver, i);
	}

	/* register the tty driver */
	retval = tty_register_driver(tiny_tty_driver);
	if (retval) {
		pr_err("failed to register tiny tty driver");
		tty_unregister_driver(tiny_tty_driver);
		return retval;
	}

	for (i = 0; i < TINY_TTY_MINORS; ++i)
		tty_register_device(tiny_tty_driver, i, NULL);

	pr_info(DRIVER_DESC " " DRIVER_VERSION);
	return retval;
}

static void __exit tiny_exit(void)
{
	struct tiny_serial *tiny;
	int i;

	for (i = 0; i < TINY_TTY_MINORS; ++i) {
		tty_unregister_device(tiny_tty_driver, i);
		tty_port_destroy(tiny_tty_port + i);
	}
	tty_unregister_driver(tiny_tty_driver);

	/* shut down all of the timers and free the memory */
	for (i = 0; i < TINY_TTY_MINORS; ++i) {
		tiny = tiny_table[i];
		if (tiny) {
			/* close the port */
			while (tiny->open_count)
				do_close(tiny);

			/* shut down our timer and free the memory */
			del_timer(&tiny->timer);
			kfree(tiny);
			tiny_table[i] = NULL;
		}
	}
}

module_init(tiny_init);
module_exit(tiny_exit);
