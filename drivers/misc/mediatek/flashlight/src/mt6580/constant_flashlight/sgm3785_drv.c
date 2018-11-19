#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/platform_device.h>
//#include <mach/mt_pm_ldo.h>
//#include <mach/mt_gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <sgm3785_drv.h>
#include "kd_flashlight.h"
#include "kd_camera_typedef.h"
#include "mt_pwm.h"
//#include "core.h"
#include <linux/io.h>
#include <linux/export.h>


struct pinctrl *sgm_gpio_ctrl = NULL;
struct pinctrl_state *sgm_gpio_default = NULL;
struct pinctrl_state *sgm_gpio_enm_mode0_h = NULL;
struct pinctrl_state *sgm_gpio_enm_mode0_l = NULL;
struct pinctrl_state *sgm_gpio_enm_mode5_h = NULL;
struct pinctrl_state *sgm_gpio_enm_mode5_l = NULL;

struct pinctrl_state *sgm_gpio_enf_mode0_h = NULL;
struct pinctrl_state *sgm_gpio_enf_mode0_l = NULL;
struct pinctrl_state *sgm_gpio_enf_mode5_h = NULL;
struct pinctrl_state *sgm_gpio_enf_mode5_l = NULL;


static void sgm3785_shutdown(struct platform_device *dev);

//S32 sgm3785_shutoff();
int sgm3785_ioctrl(U16 mode, U16 duty);

struct pwm_spec_config sgm3785_config = {
    .pwm_no = PWM_NO,
    .mode = PWM_MODE_OLD,
    .clk_div = CLK_DIV32,//CLK_DIV16: 147.7kHz    CLK_DIV32: 73.8kHz
    .clk_src = PWM_CLK_OLD_MODE_BLOCK,
    .pmic_pad = false,
    .PWM_MODE_OLD_REGS.IDLE_VALUE = IDLE_FALSE,
    .PWM_MODE_OLD_REGS.GUARD_VALUE = GUARD_FALSE,
    .PWM_MODE_OLD_REGS.GDURATION = 0,
    .PWM_MODE_OLD_REGS.WAVE_NUM = 0,
    .PWM_MODE_OLD_REGS.DATA_WIDTH = 11,
    .PWM_MODE_OLD_REGS.THRESH = 5,
};


void sgm3785_set_gpio(int PinType,int val)
{
	//TPD_DEBUG("[tpd]sgm_set_gpio\n");
	printk(KERN_INFO "[flashlight] sgm3785_set_gpio\n");
	switch(PinType) {
		case SGM_GPIO_ENM_MODE0:
			if (0 == val)
				pinctrl_select_state(sgm_gpio_ctrl, sgm_gpio_enm_mode0_l);
			else
				pinctrl_select_state(sgm_gpio_ctrl, sgm_gpio_enm_mode0_h);
			break;
				 
		case SGM_GPIO_ENM_MODE5:
			if (0 == val)
				pinctrl_select_state(sgm_gpio_ctrl, sgm_gpio_enm_mode5_h);
			else
				pinctrl_select_state(sgm_gpio_ctrl, sgm_gpio_enm_mode5_l);
			break;
			
		case SGM_GPIO_ENF_MODE0:
			if (0 == val)
				pinctrl_select_state(sgm_gpio_ctrl, sgm_gpio_enf_mode0_l);
			else 
				pinctrl_select_state(sgm_gpio_ctrl, sgm_gpio_enf_mode0_h);
			break;
			
		case SGM_GPIO_ENF_MODE5:
			if (0 == val)
				pinctrl_select_state(sgm_gpio_ctrl, sgm_gpio_enf_mode5_h);
			else
				pinctrl_select_state(sgm_gpio_ctrl, sgm_gpio_enf_mode5_l);
			break;
			
		default:
			printk("[flashlight] PinType (%d) is invalid !!",PinType);	
			break;
	}
	
}

void mt_set_flashlight_gpio(int PinType,int val)
{
	sgm3785_set_gpio(PinType,val);
} 


S32 sgm3785_shutoff(void)    
{

	// mt_pwm_disable(PWM_NO, false);
     sgm3785_set_gpio(SGM_GPIO_ENF_MODE0,0);
     sgm3785_set_gpio(SGM_GPIO_ENM_MODE0,0);
     sgm3138_dbg("[flashlight] sgm3785_shutdown");
     mdelay(5);
     return 0;
}

/* sgm function for led control*/
void sgm3785_set_flash_mode(U16 duty)
{

	sgm3138_dbg("[flashlight]set flashlight mode\n");	
/** <2016/11/07 yinyapeng modify for mt6580 not support pwm mode> **/
#if 0
	S32 ret;
	sgm3138_dbg("shenwuyi add sgm3785_set_flash_mode:duty = %d\n",duty);
	sgm3785_set_gpio(SGM_GPIO_ENF_MODE0,0);
	mdelay(2);
	sgm3785_set_gpio(SGM_GPIO_ENM_MODE5,1);
	sgm3785_config.PWM_MODE_OLD_REGS.THRESH = duty;//8;//by likai change duty to 7 
	ret = pwm_set_spec_config(&sgm3785_config);	
	mdelay(2);
	sgm3785_set_gpio(SGM_GPIO_ENF_MODE0,1);
	return ret;
#else
	sgm3785_set_gpio(SGM_GPIO_ENM_MODE0,1);
	mdelay(1);
	sgm3785_set_gpio(SGM_GPIO_ENF_MODE0,1);
#endif
	
}

void sgm3785_set_torch_mode(U16 duty)
{
	sgm3138_dbg("[flashlight]set torch mode\n");
	
/** <2016/11/07 yinyapeng modify for mt6580 not support pwm mode> **/
#if 0
	S32 ret;
	ret = 0;
	sgm3138_dbg("shenwuyi add sgm3785_set_torch_mode:,duty = %d",duty);
	sgm3785_set_gpio(SGM_GPIO_ENF_MODE0,0);
	mdelay(2);
	sgm3785_set_gpio(SGM_GPIO_ENM_MODE0,0);
	mdelay(1);
	sgm3785_set_gpio(SGM_GPIO_ENM_MODE0,1);
	mdelay(6);
	sgm3785_set_gpio(SGM_GPIO_ENM_MODE5,1);
	mdelay(2);
	sgm3785_config.PWM_MODE_OLD_REGS.THRESH = duty;
	ret = pwm_set_spec_config(&sgm3785_config);	
	return ret;
#else
	sgm3785_set_gpio(SGM_GPIO_ENF_MODE0,0);	
	mdelay(1);
	sgm3785_set_gpio(SGM_GPIO_ENM_MODE0,1);
#endif		
		
}


int sgm3785_ioctrl(U16 mode, U16 duty)
{
	if(mode < MODE_MIN || mode >= MODE_MAX)
	{
		sgm3138_dbg("[flashlight] mode error!!!\n");
		return 1;
	}
	
	if(duty < 0 || duty > 20)
	{
		sgm3138_dbg("[flashlight] duty error!!!\n");
		return 1;
	}

	switch(mode){
		case FLASH_M:
			sgm3785_set_flash_mode(duty);
			break;

		case TORCH_M:
			sgm3785_set_torch_mode(duty);
			break;

		case PWD_M:
			sgm3785_shutoff();
			break;

		default:
			sgm3138_dbg("[flashlight] error ioctr!!!\n");
			break;
	}

	return 0;
}

void sgm3785_FL_Enable(int duty)
{
	printk(KERN_INFO "[flashlight] FL enable duty =%d\n",duty);
	if(duty > 0)//flashlight mode
		sgm3785_ioctrl(FLASH_M, F_DUTY);
	else //torch mode
		sgm3785_ioctrl(TORCH_M, T_DUTY);
}

void sgm3785_FL_Disable(void)
{
	printk(KERN_INFO "[flashlight] FL_disable\n");
	sgm3785_ioctrl(PWD_M, 0);
}


#if 1


static ssize_t show_sgm3785_torch_mode(struct device *dev,struct device_attribute *attr, char *buf)
{
	  return  sprintf(buf, "show_sgm3785_torch_mode\n");
}

static ssize_t restore_sgm3785_torch_mode(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
        kal_int32 temp = 0;
        
        sscanf(buf, "%d", &temp);
        sgm3138_dbg("[flashlight] restore_sgm3785_torch_mode temp = %d\n",temp);
        if(temp < 0 || temp > 11)
        {
            
            sgm3785_shutoff();
            sgm3138_dbg("[sgm3785] duty error!!!\n");
            //return 1;
        }
        else
        {
            sgm3785_set_torch_mode(temp);
        }
        
        return size;
}


static ssize_t restore_sgm3785_flash_mode(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	kal_int32 temp = 0;
	

	sscanf(buf, "%d", &temp);
	
    sgm3138_dbg("[sgm3785] restore_sgm3785_flash_mode temp = %d\n",temp);
	

	if(temp < 0 || temp > 11)
	{
        sgm3785_shutoff();
		sgm3138_dbg("[sgm3785] duty error!!!\n");
		//return 1;
	}
    else
    {
        sgm3785_set_flash_mode(temp);
    }
    
    return size;
}


static ssize_t show_sgm3785_flash_mode(struct device *dev,struct device_attribute *attr, char *buf)
{
      return  sprintf(buf, "show_sgm3785_flash_mode\n");
}

#endif



int sgm_get_gpio_info(struct platform_device *pdev)
{
	int ret;
	printk("[flashlight] sgm_get_gpio_info\n");
	sgm_gpio_ctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(sgm_gpio_ctrl)) {
		dev_err(&pdev->dev, "Cannot find sgm gpio pinctrl!");
		ret = PTR_ERR(sgm_gpio_ctrl);
		return ret;
	}
	sgm_gpio_default = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_default");
	
	sgm_gpio_enm_mode0_h = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_enm_mode0_h");	
	if (IS_ERR(sgm_gpio_enm_mode0_h)) {
		ret = PTR_ERR(sgm_gpio_enm_mode0_h);
		pr_debug("%s : pinctrl err, sgm_gpio_enm_mode0_h \n", __func__);
	}

	sgm_gpio_enm_mode0_l = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_enm_mode0_l");	
	if (IS_ERR(sgm_gpio_enm_mode0_l)) {
		ret = PTR_ERR(sgm_gpio_enm_mode0_l);
		pr_debug("%s : pinctrl err, sgm_gpio_enm_mode0_l \n", __func__);
	}
	
	sgm_gpio_enm_mode5_h = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_enm_mode5_h");	
	if (IS_ERR(sgm_gpio_enm_mode5_h)) {
		ret = PTR_ERR(sgm_gpio_enm_mode5_h);
		pr_debug("%s : pinctrl err, sgm_gpio_enm_mode5_h \n", __func__);
	}
		
	sgm_gpio_enm_mode5_l = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_enm_mode5_l");	
	if (IS_ERR(sgm_gpio_enm_mode5_l)) {
		ret = PTR_ERR(sgm_gpio_enm_mode5_l);
		pr_debug("%s : pinctrl err, sgm_gpio_enm_mode5_l \n", __func__);
	}
	
	sgm_gpio_enf_mode0_h = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_enf_mode0_h");
	if (IS_ERR( sgm_gpio_enf_mode0_h)) {
		ret = PTR_ERR(sgm_gpio_enf_mode0_h);
		pr_debug("%s : pinctrl err,sgm_gpio_enf_mode0_h \n",__func__);
	}
	
	sgm_gpio_enf_mode0_l = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_enf_mode0_l");
	if (IS_ERR( sgm_gpio_enf_mode0_l)) {
		ret = PTR_ERR(sgm_gpio_enf_mode0_l);
		pr_debug("%s : pinctrl err,sgm_gpio_enf_mode0_l \n",__func__);
	}
	
	sgm_gpio_enf_mode5_h = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_enf_mode5_h");
	if (IS_ERR( sgm_gpio_enf_mode5_h)) {
		ret = PTR_ERR(sgm_gpio_enf_mode5_h);
		pr_debug("%s : pinctrl err,sgm_gpio_enf_mode5_h \n",__func__);
	}
	
	sgm_gpio_enf_mode5_l = pinctrl_lookup_state(sgm_gpio_ctrl, "flashlight_enf_mode5_l");
	if (IS_ERR( sgm_gpio_enf_mode5_l)) {
		ret = PTR_ERR(sgm_gpio_enf_mode5_l);
		pr_debug("%s : pinctrl err,sgm_gpio_enf_mode5_l \n",__func__);
	}
	
	printk("[flashlight_gpio%d] flashlight_gpio_pinctrl----------\n", pdev->id);
	return 0;
}

static DEVICE_ATTR(sgm3785_torch_mode,  0664, show_sgm3785_torch_mode, restore_sgm3785_torch_mode);
static DEVICE_ATTR(sgm3785_flash_mode,  0664, show_sgm3785_flash_mode, restore_sgm3785_flash_mode);

static int sgm3785_probe(struct platform_device *pdev)
{ 
 
 int ret_device_file=0;
  sgm_get_gpio_info(pdev);
  ret_device_file = device_create_file(&(pdev->dev), &dev_attr_sgm3785_flash_mode);
  ret_device_file = device_create_file(&(pdev->dev), &dev_attr_sgm3785_torch_mode);
  return 0;
}
/*
static int sgm3785_probe(struct platform_device *dev)
{ 

  int ret_device_file=0;
  sgm3138_dbg("isgm3785_prob");
  hw_init();
  ret_device_file = device_create_file(&(dev->dev), &dev_attr_sgm3785_flash_mode);
  ret_device_file = device_create_file(&(dev->dev), &dev_attr_sgm3785_torch_mode);
 
  return 0;
}
*/
static int sgm3785_remove(struct platform_device *dev)    
{
    sgm3138_dbg("[flashlight]sgm3785_remove");
    return 0;
}
static int sgm3785_suspend(struct platform_device *dev, pm_message_t state)    
{
    sgm3138_dbg("[flashlight]sgm3785_suspend");   
    return 0;
}

static int sgm3785_resume(struct platform_device *dev)
{
	
   sgm3138_dbg("[flashlight]sgm3785_resume"); 
   return 0;
}

static void sgm3785_shutdown(struct platform_device *dev)    
{
	sgm3138_dbg("[flashlight]sgm3785_shutdown\n");
     mdelay(5);	 
}

struct platform_device flash_leds_sgm3785_device = {
    .name   = "sgmxx",
    .id	    = -1,
};

static const struct of_device_id sgm_gpio_of_match[] = {
{.compatible = "mediatek,sgmxx",},
{}
};
static struct platform_driver flash_leds_sgm3785_driver = {
    .probe       = sgm3785_probe,
    .remove      = sgm3785_remove,
    .shutdown    = sgm3785_shutdown,
    .suspend     = sgm3785_suspend,
    .resume      = sgm3785_resume,
    .driver      = {
        .name = "sgm3785",
   
    #ifdef CONFIG_OF
       .of_match_table = sgm_gpio_of_match,
	#endif
	}
};

/************ sgm end*********************/


static int flash_leds_sgm3785_init(void)
{
 
    int ret;
/*
    ret = platform_device_register(&flash_leds_sgm3785_device);
    if (ret) {
    printk("<<-sgm3785->> Unable to register device (%d)\n", ret);
	return ret;
    }
*/   
    printk("[flashlight]flash_leds_sgm3785_init\n");
    ret = platform_driver_register(&flash_leds_sgm3785_driver);
    if (ret) {
    	printk("[flashlight] Unable to register sgm3785 driver (%d)\n", ret);
	return ret;
    }
    return 0;    
}

static void flash_leds_sgm3785_exit(void)
{
	sgm3138_dbg("[flashlight]flash_leds_sgm3785_exit");
	//platform_device_unregister(&flash_leds_sgm3785_device);
	platform_driver_unregister(&flash_leds_sgm3785_driver);

}

fs_initcall(flash_leds_sgm3785_init);
module_exit(flash_leds_sgm3785_exit);

EXPORT_SYMBOL(sgm3785_set_gpio);
EXPORT_SYMBOL(sgm3785_shutdown);
EXPORT_SYMBOL(sgm3785_set_torch_mode);
MODULE_AUTHOR("shenwuyi@huaqin.com");
MODULE_DESCRIPTION("Flash Led Sgm3785 Device Driver");
MODULE_LICENSE("GPL");

