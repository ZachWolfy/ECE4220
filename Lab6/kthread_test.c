/* Based on example from: http://tuxthink.blogspot.com/2011/02/kernel-thread-creation-1.html
   Modified and commented by: Luis Rivera			
   
   Compile using the Makefile
*/

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif
   
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>	// for kthreads
#include <linux/sched.h>	// for task_struct
#include <linux/time.h>		// for using jiffies 
#include <linux/timer.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 	//for copy_from_user

MODULE_LICENSE("GPL");

#define ADD_BASE 0x3F200000 //address base for GPIO
#define GPFSEL1_OFFSET 0X04/4 //GPSET0 offset from GPIO address base
#define GPFSEL2_OFFSET 0X08/4 //GPSET0 offset from GPIO address base
#define GPSET0_OFFSET 0X01C/4 //GPSET0 offset from GPIO address base
#define GPCLR0_OFFSET 0X28/4 //GPCLR0 offset from GPIO address base
#define GPEDS0_OFFSET 0x40/4 //GPEDS0 offset from GPIO address base
#define GPAREN0_OFFSET 0x7C/4 //GPAREN0 offset from GPIO address base
#define GPPUD_OFFSET 0x94/4 //GPPUD offset from GPIO address base
#define GPPUDCLK0_OFFSET 0x98/4 //GPPUDCLK0 offset from GPIO address base

#define VALUE 0b111110000000000000000
#define MSG_SIZE 50
#define CDEV_NAME "Lab6"	// "YourDevName"

// structure for the kthread.
static struct task_struct *kthread1;

int mydev_id;
int freq;
static int major; 
static char msg[MSG_SIZE];

static irqreturn_t button_isr(int irq, void *dev_id)
{
	//diable 79
	disable_irq_nosync(79);
	
	//GPIO address declaration
	unsigned long *GPFSEL0 = (unsigned long*)ioremap(ADD_BASE, 4096);
	unsigned long *GPEDS0 = GPFSEL0 + GPEDS0_OFFSET;
	
	//check button
	unsigned long button = ioread32(GPEDS0);
	printk("%X\n",button);

	if(button == 0x10000)
	{
		freq = 1000;
	}	
	else if(button == 0x20000)
	{
		freq = 750;
	}
	else if(button == 0x40000)
	{
		freq = 500;
	}
	else if(button == 0x80000)
	{
		freq = 250;
	}
	else if(button == 0x100000)
	{
		freq = 100;
	}

	//clear interrupt
	iowrite32(VALUE, GPEDS0);
	

	printk("Interrupt handled\n");

	enable_irq(79);
	
	return IRQ_HANDLED;
}
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
	ssize_t dummy;
	
	if(len > MSG_SIZE)
		return -EINVAL;
	
	// unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
	dummy = copy_from_user(msg, buff, len);	// Transfers the data from user space to kernel space
	if(len == MSG_SIZE)
		msg[len-1] = '\0';	// will ignore the last character received.
	else
		msg[len] = '\0';
	
	// You may want to remove the following printk in your final version.
	printk("Message from user space: %s\n", msg);
	if(msg[0] == 'A')
	{
		freq = 1000;
	}	
	else if(msg[0] == 'B')
	{
		freq = 750;
	}
	else if(msg[0] == 'C')
	{
		freq = 500;
	}
	else if(msg[0] == 'D')
	{
		freq = 250;
	}
	else if(msg[0] == 'E')
	{
		freq = 100;
	}
	
	return len;		// the number of bytes that were written to the Character Device.
}
// structure needed when registering the Character Device. Members are the callback
// functions when the device is read from or written to.
static struct file_operations fops = {
	.write = device_write,
};
// Function to be associated with the kthread; what the kthread executes.
int kthread_fn(void *ptr)
{
	unsigned long *GPFSEL0 = (unsigned long*)ioremap(ADD_BASE, 4096);
	unsigned long *GPSET0 = GPFSEL0 + GPSET0_OFFSET;
	unsigned long *GPCLR0 = GPFSEL0 + GPCLR0_OFFSET;
	
	unsigned long j0, j1;
	int count = 0;

	printk("In kthread1\n");
	j0 = jiffies;		// number of clock ticks since system started;
						// current "time" in jiffies
	j1 = j0 + 1*HZ;	// HZ is the number of ticks per second, that is
						// 1 HZ is 1 second in jiffies
	
	while(time_before(jiffies, j1))	// true when current "time" is less than j1
        schedule();		// voluntarily tell the scheduler that it can schedule
						// some other process
	
	printk("Before loop\n");
	
	// The ktrhead does not need to run forever. It can execute something
	// and then leave.
	while(1)
	{
		//generate square wave sound
		iowrite32(1 << 6, GPSET0);
		udelay(freq);
		//turns off the speaker
		iowrite32(1 << 6, GPCLR0);
		udelay(freq);
	
		//msleep(1000);	// good for > 10 ms
		//msleep_interruptible(1000); // good for > 10 ms
		//udelay(unsigned long usecs);	// good for a few us (micro s)
		//usleep_range(unsigned long min, unsigned long max); // good for 10us - 20 ms
		
		
		// In an infinite loop, you should check if the kthread_stop
		// function has been called (e.g. in clean up module). If so,
		// the kthread should exit. If this is not done, the thread
		// will persist even after removing the module.
		if(kthread_should_stop()) {
			do_exit(0);
		}
				
		// comment out if your loop is going "fast". You don't want to
		// printk too often. Sporadically or every second or so, it's okay.
		//printk("Count: %d\n", ++count);
	}
	
	return 0;
}
int thread_init(void)
{
	major = register_chrdev(0, CDEV_NAME, &fops);
	if (major < 0) {
     		printk("Registering the character device failed with %d\n", major);
	     	return major;
	}
	printk("Lab6_cdev_kmod example, assigned major: %d\n", major);
	printk("Create Char Device (node) with: sudo mknod /dev/%s c %d 0\n", CDEV_NAME, major);
	
	char kthread_name[11]="my_kthread";	// try running  ps -ef | grep my_kthread
						// when the thread is active.
	printk("In init module\n");
    	
    	int dummy = 0;
	
	//GPIO address declaration
	unsigned long *GPFSEL0 = (unsigned long*)ioremap(ADD_BASE, 4096);
	unsigned long *GPFSEL1 = GPFSEL0 + GPFSEL1_OFFSET;
	unsigned long *GPFSEL2 = GPFSEL0 + GPFSEL2_OFFSET;
	unsigned long *GPAREN0 = GPFSEL0 + GPAREN0_OFFSET;
	unsigned long *GPPUD = GPFSEL0 + GPPUD_OFFSET;
	unsigned long *GPPUDCLK0 = GPFSEL0 + GPPUDCLK0_OFFSET;
	
	//set output to BCM6
	iowrite32(1 << 18, GPFSEL0);
	
	//set input BCM16-20
	iowrite32(0x0, GPFSEL1);
	iowrite32(0x0, GPFSEL2);
	
	//configure to trigger interrupt (01 for pull down)
	iowrite32(0b01, GPPUD);
	iowrite32(0b01,GPPUDCLK0);
	udelay(100);
	
	//rising edge sensitive pull down - set gparen0
	iowrite32(VALUE,GPAREN0);
	
	//ISR_my bind service routine(channel 79) calling request_irq
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	
	//enable(79)
	enable_irq(79);
	
	kthread1 = kthread_create(kthread_fn, NULL, kthread_name);

	if((kthread1))	// true if kthread creation is successful
	{
		printk("Inside if\n");
			// kthread is dormant after creation. Needs to be woken up
		wake_up_process(kthread1);
	}

    return 0;
}

void thread_cleanup(void) {

	int ret;
	// the following doesn't actually stop the thread, but signals that
	// the thread should stop itself (with do_exit above).
	// kthread should not be called if the thread has already stopped.
	ret = kthread_stop(kthread1);
								
	if(!ret)
		printk("Kthread stopped\n");
		
	free_irq(79, &mydev_id);
	unregister_chrdev(major, CDEV_NAME);
}

module_init(thread_init);
module_exit(thread_cleanup);
