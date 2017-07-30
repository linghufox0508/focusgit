#include"linux/cdev.h"
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/mm.h>
#include<linux/sched.h>
#include<linux/init.h>
#include<asm/io.h>
//#include<asm/system.h>
#include<asm/uaccess.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/slab.h>
#include<linux/proc_fs.h>
#include<asm/unistd.h>
#include<linux/string.h>


#define GLOBALMEM_SIZE 0x1000
#define MEM_CLEAR 0x1
#define GLOBALMEM_MAJOR 250

static const char proc_filename[]="driver/cdev_test";
static const char file_path[]="/proc/";

static int globalmem_major = GLOBALMEM_MAJOR;


struct globalmem_dev{
       struct cdev cdev;
       unsigned char mem[GLOBALMEM_SIZE]; 

}; 

struct globalmem_dev *globalmem_devp;

int globalmem_open(struct inode *inode,struct file *filp)
{
    printk("globalmem_open\n");
    filp->private_data = globalmem_devp;
    return 0;
}

int globalmem_release(struct inode *inode,struct file *filp)
{
    return 0;
}



static int globalmem_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,unsigned long arg)
{
    struct globalmem_dev *dev = filp->private_data;
    
    printk("globalmem_ioctl\n");
    switch(cmd){
        case MEM_CLEAR:
            memset(dev->mem,0,GLOBALMEM_SIZE);
            printk(KERN_INFO "globalmem is set to 0\n");
            break;

        default:
             return -EINVAL;
    }

    return 0;
}

static ssize_t globalmem_read(struct file *filp,char __user *buf,size_t size,loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;

    struct globalmem_dev *dev = filp->private_data;
     
    printk("globalmem_read\n");
    if (count >= GLOBALMEM_SIZE)
        return 0;
    
    if (count > GLOBALMEM_SIZE -p)
       count = GLOBALMEM_SIZE - p;
     
   
    if (copy_to_user(buf,(void *) (dev->mem + p),count)) {
        ret = -EFAULT;
    } else {
        *ppos += count;
        ret = count;
        printk(KERN_INFO "read %u bytes(s) from %lu\n ",count,p);
        
    }
    
    
    return ret;
}




static ssize_t globalmem_write(struct file *filp, const char __user *buf,size_t size,loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;

    struct globalmem_dev *dev = filp->private_data;

    if (p >= GLOBALMEM_SIZE)
        return 0;

    if (count > GLOBALMEM_SIZE - p)
        count = GLOBALMEM_SIZE - p;


    if (copy_from_user(dev->mem + p,buf,count))
       ret = - EFAULT;
    else {
       *ppos += count;
       ret = count;
       printk(KERN_INFO "written %u bytes(s) from %lu\n",count,p);

    }
    
    printk(" globalmem_write\n");
    return ret;
}

static const struct file_operations globalmem_fops = {
       .owner = THIS_MODULE,
       .read = globalmem_read,
       .write = globalmem_write,
       .unlocked_ioctl = globalmem_ioctl,
       .open = globalmem_open,
       .release = globalmem_release,
};

static void globalmem_setup_cdev(struct globalmem_dev *dev,int index)
{
    int err,devno = MKDEV(globalmem_major,index);
    
    cdev_init(&dev->cdev,&globalmem_fops);
    dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&dev->cdev,devno,1);
     
    proc_create(proc_filename,0666,NULL,&globalmem_fops);
    if (err)
         printk(KERN_NOTICE "error %d adding globalmem %d",err,index);
}

int globalmem_init(void)
{
    int result;
    dev_t devno = MKDEV(globalmem_major,0);

    if (globalmem_major)
       result = register_chrdev_region(devno,1,"globalmem");
    else {
       result = alloc_chrdev_region(&devno,0,1,"globalmem");
       globalmem_major = MAJOR(devno);
    }
    
    if (result < 0)
       return result;


    globalmem_devp = kmalloc(sizeof(struct globalmem_dev),GFP_KERNEL);
    if(!globalmem_devp) {
       result = -ENOMEM;
       goto fail_malloc;
    }
     
    memset(globalmem_devp,0,sizeof(struct globalmem_dev));
    globalmem_setup_cdev(globalmem_devp,0);
    
    return 0;

fail_malloc:
    unregister_chrdev_region(devno,1);
    return result;
}

void globalmem_exit(void)
{
    cdev_del(&globalmem_devp->cdev);
    kfree(globalmem_devp);
    remove_proc_entry("driver/cdev_test",NULL);
    unregister_chrdev_region(MKDEV(globalmem_major,0),1);
}

MODULE_AUTHOR("YINYAPENG");
MODULE_LICENSE("Dual BAS/GPL");

//module_param(globalmem_major,init,S_IRUGO);
module_init(globalmem_init);
module_exit(globalmem_exit);
