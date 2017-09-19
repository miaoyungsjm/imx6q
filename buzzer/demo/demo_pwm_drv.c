#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/gpio.h>
#include <linux/pwm.h>

//#include <plat/gpio-cfg.h>

#define PWM_ON  0x100001
#define PWM_OFF 0x100002

struct pwm_device *pwm_dev_2;
struct pwm_device *pwm_dev_3;

static long pwm_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    int ret;
    switch(cmd) {
        case PWM_ON:
                ret = pwm_config(pwm_dev_2,200000,500000);
                if(ret < 0){
                    printk("pwm_dev_2 ioctl fail");
                    return 0;
                }
                ret = pwm_config(pwm_dev_3,300000,500000);
                if(ret < 0){
                    printk("pwm_dev_3 ioctl fail");
                }
                pwm_enable(pwm_dev_2);
                pwm_enable(pwm_dev_3);
            break;
        case PWM_OFF:
                ret = pwm_config(pwm_dev_2,0,500000);
                if(ret < 0){
                    printk("pwm_dev_2 ioctl fail");
                    return 0;
                }
                ret = pwm_config(pwm_dev_3,0,500000);
                if(ret < 0){
                    printk("pwm_dev_3 ioctl fail");
                }
                pwm_disable(pwm_dev_2);
                pwm_disable(pwm_dev_3);
            break;
    }
    return 0;
}
//定义初始化硬件操作方法
static struct file_operations pwm_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = pwm_ioctl
};
//定义初始化混杂设备对象
static struct miscdevice pwm_misc = {
    .minor = MISC_DYNAMIC_MINOR, //动态分配次设备号
    .name = "mypwm",             //dev/mypwm
    .fops = &pwm_fops 
};
static int pwm_init(void)
{
    int ret;
    printk("regisger pwm_misc device\n");
    //1.申请pwm资源，设置输出为0
    pwm_dev_2 = pwm_request(1,"pwm_2");
    if(pwm_dev_2 == NULL){
        printk("pwm_dev_2 register fail\n");
    }
    pwm_dev_3 = pwm_request(2,"pwm_3");
    if(pwm_dev_3 == NULL){
        printk("pwn_dev_3 register fail\n");
    }


    ret = pwm_config(pwm_dev_2,0,500000);
    if(ret < 0){
        printk("pwm_config_2 init fail\n");
        return 0;
    }
    ret = pwm_config(pwm_dev_3,0,500000);
    if(ret < 0){
        printk("pwm_config_3 init fail\n");
        return 0;
    }

    ret = pwm_enable(pwm_dev_2);
    if(ret == 0){
        printk("pwm_enable_dev_2 init success\n");
    }
    if(ret < 0 ){
        printk("pwm_enable_dev_2 init fail\n");
        return 0;
    }
    ret = pwm_enable(pwm_dev_3);
    if(ret == 0){
        printk("pwm_enable_dev_3 init success\n");
    }
    if(ret < 0 ){
        printk("pwm_enable_dev_3 init fail\n");
        return 0;
    }
    //2.注册混杂设备
    misc_register(&pwm_misc);
    return 0;
}

static void pwm_exit(void)
{
    printk("unregister pwm_misc device\n");
    //1.卸载混杂设备
    misc_deregister(&pwm_misc);
    //2.释放pwm资源
    pwm_config(pwm_dev_2,0,500000);
    pwm_disable(pwm_dev_2);
    pwm_free(pwm_dev_2);
    
    pwm_config(pwm_dev_3,0,500000);
    pwm_disable(pwm_dev_3);
    pwm_free(pwm_dev_3);
}
module_init(pwm_init);
module_exit(pwm_exit);
MODULE_LICENSE("GPL");
