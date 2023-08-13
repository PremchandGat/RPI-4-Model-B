#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/timer.h>



static struct timer_list my_timer;

void my_timer_callback(struct timer_list *timer) {
  printk(KERN_ALERT "This line is printed after 5 seconds.\n");
}

static int init_module_with_timer(void) {
  printk(KERN_ALERT "Initializing a module with timer.\n");

  /* Setup the timer for initial use. Look in linux/timer.h for this function */
  timer_setup(&my_timer, my_timer_callback, 0);
  mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

  return 0;
}

static void exit_module_with_timer(void) {
  printk(KERN_ALERT "Goodbye, cruel world!\n");
  del_timer(&my_timer);
}

module_init(init_module_with_timer);
module_exit(exit_module_with_timer);
MODULE_LICENSE("GPL");
