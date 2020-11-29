#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define ADD_BASE 0x3F200000 //address base for GPIO
#define GPSET0_OFFSET 0X07 //GPSET0 offset from GPIO address base
#define GPCLR0_OFFSET 0X0A //GPCLR0 offset from GPIO address base
#define GPEDS0_OFFSET 0x10 //GPEDS0 offset from GPIO address base
#define GPAREN0_OFFSET 0x1F //GPAREN0 offset from GPIO address base
#define GPPUD_OFFSET 0x25 //GPPUD offset from GPIO address base
#define GPPUDCLK0_OFFSET 0x26 //GPPUDCLK0 offset from GPIO address base

#define VALUE 0b10000000000000000

int mydev_id;

//Interupt handler
static irqreturn_t button_isr(int irq, void *dev_id)
{

	//diable 79
	disable_irq_nosync(79);
	/*
	//GPIO address declaration
	unsigned long *GPFSEL0 = (unsigned long*)ioremap(ADD_BASE, 4096);
	//unsigned long *GPSET0 = GPFSEL0 + GPSET0_OFFSET;
	//unsigned long *GPCLR0 = GPFSEL0 + GPCLR0_OFFSET;
	unsigned long *GPEDS0 = GPFSEL0 + GPEDS0_OFFSET;
	//check button
	unsigned long button = ioread32(GPEDS0);
	//printk("%X",button);
	//clear interrupt
	iowrite32(VALUE, GPEDS0);
	printk("Interrupt handled\n");
	*/
	enable_irq(79);
	
	return IRQ_HANDLED;
}

int my_init(void)
{
	int dummy = 0;
	
	//GPIO address declaration
	unsigned long *GPFSEL0 = (unsigned long*)ioremap(ADD_BASE, 4096);
	unsigned long *GPAREN0 = GPFSEL0 + GPAREN0_OFFSET;
	unsigned long *GPPUD = GPFSEL0 + GPPUD_OFFSET;
	unsigned long *GPPUDCLK0 = GPFSEL0 + GPPUDCLK0_OFFSET;

	//set input bcm16-20
	iowrite32(0x0, GPFSEL0);
	
	//configure to trigger interrupt (01 for pull down)
	/*iowrite32(0b01, GPPUD);
	iowrite32(0b01, GPPUDCLK0);
	udelay(100);
	iowrite32(VALUE,GPPUDCLK0);
	udelay(100);
	*/
	//rising edge sensitive pull down - set gparen0
	iowrite32(VALUE, GPAREN0);

	//ISR_my bind service routine(channel 79) calling request_irq
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	
	//enable(79)
	enable_irq(79);

	printk("Button Detection enabled.\n");
	return 0;
}

void my_cleanup(void)
{
	//free_irq
	free_irq(79, &mydev_id);
	
	printk("Button Detection disabled.\n");
}

module_init(my_init);
module_exit(my_cleanup);
