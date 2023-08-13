#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x122c3a7e, "_printk" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x15ba50a6, "jiffies" },
	{ 0x24d273d1, "add_timer" },
	{ 0xdc0e4855, "timer_delete" },
	{ 0x6919c64a, "__tty_alloc_driver" },
	{ 0x67b27ec1, "tty_std_termios" },
	{ 0x6f0121e9, "tty_register_driver" },
	{ 0xcae2cae0, "tty_port_init" },
	{ 0xd46d62e5, "tty_port_register_device" },
	{ 0x17a90584, "tty_driver_kref_put" },
	{ 0xcd31e06d, "tty_port_destroy" },
	{ 0xbbedba34, "tty_unregister_driver" },
	{ 0x11c59d62, "tty_unregister_device" },
	{ 0x99b85dd6, "__tty_insert_flip_char" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0xb3d1bec3, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F8F64BB58F32246F2551152");
