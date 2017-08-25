#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <linux/jbd2.h>

static struct kprobe kp;

static int k_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
	trace_printk("entering %s kprobe \n",p->symbol_name);
    /*
     * if you return 1. you need to modify pt_regs->ip.
     */
    return 0;
}

static void k_post_handler(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{
	trace_printk("exciting %s kprobe \n", p->symbol_name);
}

static int k_fault_handler(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	trace_printk("exception occured in %s, trap number : %dn",p->symbol_name, trapnr);
	return 0;
}

static struct jprobe jp;

/* jprobe init function 
 * TODO
 *  - you need to insert jprobe entry function here.
 */


static int __init init_probe_module(void)
{
	int ret;

    /* init kprobe structure */
    /* TODO
     * - you need to insert eithrer addr or symbol name in kprobe structure.
     *
     */

	kp.pre_handler = k_pre_handler;
	kp.post_handler = k_post_handler;
	kp.fault_handler = k_fault_handler;

    /* register kprobe */
	ret = register_kprobe(&kp);
	if (ret < 0) {
		trace_printk("register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	trace_printk("%s kprobe registered \n", kp.symbol_name);

    /* init jprobe structure */
    /*TODO
     *  - you need to inserrt either addr or symbol name in kprobe structure in jprobe.
     *  - you need to set jprobe entry point which you declared.
     */

    /* register jprobe */
	ret = register_jprobe(&jp);
	if (ret < 0) {
		trace_printk(KERN_INFO "register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	trace_printk("%s jprobe registered \n", jp.kp.symbol_name);

	return 0;
}

static void __exit exit_probe_module(void)
{
    /* unregister kprobe */
	unregister_kprobe(&kp);
    /* unregister kprobe */
 	unregister_jprobe(&jp); 
	trace_printk("%s kprobe unregistered \n", kp.symbol_name);
	trace_printk("%s jprobe unregistered \n", jp.kp.symbol_name);
}


module_init(init_probe_module)
module_exit(exit_probe_module)
MODULE_LICENSE("GPL");
