#include<linux/module.h>

static int __init helloWorldDriver(void){
    pr_info("Hello Premchand\n");
    return 0;
}

static void __exit helloWorldCleanup(void){
    pr_info("Bye! Nice to meet you\n");
}

module_init(helloWorldDriver);
module_exit(helloWorldCleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PREMCHAND");
MODULE_DESCRIPTION("This is my first Kernel Module");
MODULE_INFO(board, "RPI 4 Model B");

