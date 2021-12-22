# LaboKernel

/* COMPILE */

make

sudo insmod main.ko


/* MIJN PARAMETERS */

sudo insmod main.ko ledjes=5,22 states=1,0 toggel=0,5


/* MESSAGE CHECK */

dmesg


/* REMOVE */

sudo rmmod main.ko
