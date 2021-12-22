/*
* main.c − The Hardest Labo oprdacht.
*/
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/kthread.h>
/* toggel speed met threads*/
/* need 2 bc they can only toggel 1 led each */
static struct task_struct *t1 = NULL;
static struct task_struct *t2 = NULL;
/* 2 int we fill in later with toggelspeedpar */
static int del1=500;
static int del2=1000;
/* GPIOS */
static struct gpio leds[] = 
{
		{17, GPIOF_OUT_INIT_HIGH, "LED 1" },
		{22, GPIOF_OUT_INIT_HIGH, "LED 2" },
};
/* fancy stuff */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module using GPIOs to drive LEDs");

static int ledjes[2] = { -1, -1 };
static int ledjeslen = 0;

static int states[2] = { 1, 1 };
static int stateslen = 0;

static int toggel[2] = { 2, 2 };
static int toggellen = 0;

/*
* module_param_array(name, type, num, perm);
* The first param is the parameter's (in this case the array's) name
* The second param is the data type of the elements of the array
*/
module_param_array(ledjes, int, &ledjeslen, 0000);
MODULE_PARM_DESC(ledjes, "An array of integers");
module_param_array(states, int, &stateslen, 0000);
MODULE_PARM_DESC(states, "An array of integers");
module_param_array(toggel, int, &toggellen, 0000);
MODULE_PARM_DESC(toggel, "An array of integers");

/* T Functions */
// First t function
static int FunctionT1(void *data)
{
while(!kthread_should_stop())
{
    gpio_set_value(leds[0].gpio, !gpio_get_value(leds[0].gpio));
    mdelay(del1);
}
return 0;
}
// Second t function
static int FunctionT2(void *data)
{
while(!kthread_should_stop())
{
    gpio_set_value(leds[1].gpio, !gpio_get_value(leds[1].gpio));
    mdelay(del2);
}
return 0;
}


static int __init gpiomod_init(void)
{
/* Printing in terminal code */
int i;
int ret = 0;
printk(KERN_INFO "Hello world\n");
printk(KERN_INFO "%s\n", __func__);
printk(KERN_INFO "--- INPUTS ---\n");
//ledjes ARRAY PRINT
for (i = 0; i < (sizeof ledjes / sizeof (int)); i++)
{
printk(KERN_INFO "LEDJES[%d] = %d\n", i, ledjes[i]);
}
printk(KERN_INFO "got %d integers for ledjes.\n", ledjeslen);
//STATES ARRAY PRINT
for (i = 0; i < (sizeof states / sizeof (int)); i++)
{
printk(KERN_INFO "STATES[%d] = %d\n", i, states[i]);
}
printk(KERN_INFO "got %d integers for states.\n", stateslen);
//TOGGEL ARRAY PRINT
for (i = 0; i < (sizeof toggel / sizeof (int)); i++)
{
printk(KERN_INFO "TOGGEL[%d] = %d\n", i, toggel[i]);
}
printk(KERN_INFO "got %d integers for toggel.\n", toggellen);
/* Struct defining pins, direction and inital state */
for (i = 0; i < (sizeof ledjes / sizeof (int)); i++)
{
    leds[i].gpio = ledjes[i];
}
// register LED GPIOs, turn LEDs on
ret = gpio_request_array(leds, ARRAY_SIZE(leds));

if (ret) 
{
		printk(KERN_ERR "Unable to request GPIOs: %d\n", ret);
}
// toggel code with parmeters reading
for(i = 0; i < ARRAY_SIZE(leds); i++) 
{
	
	gpio_set_value(leds[i].gpio,states[i]);
	
}
/* toggel met T wanneer toggelspeed niet 0 is voor leds[0]*/
if(toggel[0] != 0)
{
    del1 = toggel[0]*500;
    t1 = kthread_create(FunctionT1, NULL, "FunctionT1");
    if(t1) 
	{
        wake_up_process(t1);
    }
        else 
		{
            printk(KERN_ERR "Cannot create FunctionT1\n");
        }
}
/* toggel met T wanneer toggelspeed niet 0 is voor leds[1]*/
if(toggel[1] != 0)
{
    del2 = toggel[1]*500;
    t2 = kthread_create(FunctionT2, NULL, "FunctionT2");
    if(t2) 
	{
        wake_up_process(t2);
    }
        else 
		{
            printk(KERN_ERR "Cannot create FunctionT2\n");
        }
}


return ret;
/*
* A non 0 return means init_module failed; module can't be loaded.
*/
//return 0;
}

static void __exit gpiomod_exit(void)
{
int i;
printk(KERN_INFO "%s\n", __func__);
/* close threads */
if(t1) {kthread_stop(t1);}
if(t2) {kthread_stop(t2);}
/* Toggel off when rmmod */
for(i = 0; i < ARRAY_SIZE(leds); i++) 
{
		gpio_set_value(leds[i].gpio,0);
}
// unregister all GPIOs
gpio_free_array(leds, ARRAY_SIZE(leds));


printk(KERN_INFO "Goodbye world 1.\n");
}

module_init(gpiomod_init);
module_exit(gpiomod_exit);