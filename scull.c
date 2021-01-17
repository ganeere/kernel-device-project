#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

// #include <asm/system.h>		/* cli(), *_flags */
#include <asm/switch_to.h>		/* cli(), *_flags */		// UPDATE FOR NEW KERNEL
// #include <asm/uaccess.h>	/* copy_*_user */
#include <linux/uaccess.h>	/* copy_*_user */				// UPDATE FOR NEW KERNEL

#include "scull_ioctl.h"

#define SCULL_MAJOR 0
#define SCULL_NR_DEVS 4
#define SCULL_QUANTUM 4000
#define SCULL_QSET 1000

// MY IOCTL VARIABLE		//////////////////////////////////////////////////////////////////
// int32_t value = 0;		

// int data_written = 0;

// char PERMUT_KEY[20] = "abcd";	
// char draft_string_[20] = "zzzz";
char *PERMUT_KEY;
char *draft_string_; 
struct scull_dev *dev_for_trim;	
//char PERMUT_KEY[20] ="ceayf";	
int PERMUT_KEY_SIZE=0;
int ORDER_KEY[20];	
int get_order_of_key(void);
// char* apply_encryption(char* input_str);
void apply_encryption(char* input_str, char* returned_encrypted_str);
void apply_decryption(char* input_str, char* returned_encrypted_str);
///////////////////////////////////////////////////////////////////////////////////////////////////

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET;

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

MODULE_AUTHOR("Alessandro Rubini, Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev {
    char **data;
    int quantum;
    int qset;
    unsigned long size;
    struct semaphore sem;
    struct cdev cdev;
	
	int data_written; //ekledik
};

struct scull_dev *scull_devices;


int scull_trim(struct scull_dev *dev)
{
    int i;

    if (dev->data) {
        for (i = 0; i < dev->qset; i++) {
            if (dev->data[i])
                kfree(dev->data[i]);
        }
        kfree(dev->data);
    }
    dev->data = NULL;
    dev->quantum = scull_quantum;
    dev->qset = scull_qset;
    dev->size = 0;
    
	dev->data_written = 0;			//MY ADDITION
    
	
	return 0;
}


int scull_open(struct inode *inode, struct file *filp)
{
    struct scull_dev *dev;

    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev;

    /* trim the device if open was write-only */
    if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
        // scull_trim(dev);// neden aldýk?
        up(&dev->sem);
    }
    return 0;
}


int scull_release(struct inode *inode, struct file *filp)
{
    return 0;
}


/*
	ENCRYPTION AND DECRYPTION FUNCTIONS

*/
int get_order_of_key(void){
	printk(KERN_INFO "INSIDE FUNCTION get_order_of_key\n");	
	// ORDER_KEY
	// int PERMUT_KEY_SIZE = 0;
	int counter=0;
	int i, j;
	
	// printk(KERN_INFO "get_order_of_key_ STRLEN IS = %d\n",strlen(PERMUT_KEY));
	for(i=0;i<sizeof(PERMUT_KEY)/sizeof(char);i++){
		if(PERMUT_KEY[i] != '\0'){
			printk(KERN_INFO "INSIDE LOPP = %c\n", PERMUT_KEY[i]);
			counter = counter + 1;
		}		
	}
	PERMUT_KEY_SIZE = counter;
	PERMUT_KEY_SIZE = strlen(PERMUT_KEY);
	
	
	// python algorithm
	// PERMUT_KEY="ceayf"
	// list_ORDER_KEY = []

	// order_current=1;
	// for i in range(0, len(PERMUT_KEY)):
		// for j in range(0, len(PERMUT_KEY)):
			// if(PERMUT_KEY[i] > PERMUT_KEY[j]):
				// order_current += 1
				// print(PERMUT_KEY[i], PERMUT_KEY[j])
		// list_ORDER_KEY.append(order_current)
		// order_current = 1
	
	int order_current=1;
	for(i=0;i<PERMUT_KEY_SIZE;i++){		
		for(j=0;j<PERMUT_KEY_SIZE;j++){		
			if(PERMUT_KEY[i] > PERMUT_KEY[j]){
				order_current = order_current + 1;
			}
		}	
		ORDER_KEY[i] = order_current;
		order_current = 1;
	}	
	
	// PRINT FOR DEBUGGING
	// for(i=0;i<PERMUT_KEY_SIZE;i++){	
		// printk(KERN_INFO "INSIDE LOPP = %d\n", ORDER_KEY[i]);
		
	// }

	
	return 0;
}

/*
	ENCRYPT STRING AND RETURN THE VALUE
*/
// char* apply_encryption(char* input_str){
void apply_encryption(char* input_str_, char* returned_encrypted_str){
	
	
	// char input_str[21] = "oneringtorulethemal0";
	int k;
	// printk(KERN_INFO "strlen(input_str_): %d \n", strlen(input_str_));
	// for(k=0;k<strlen(input_str_)-1;k++)
		// printk(KERN_INFO "%c \n", input_str_[k]);
	
	// char input_str[21] = "oneringtorulethemal";
	// strcat(input_str, "0");
	
	// char input_str[30];
	char *input_str;
	input_str = kmalloc(30 * sizeof(char), GFP_KERNEL);
	memset(input_str, 0, 30 * sizeof(char));
	// strcpy(input_str, input_str_);
	// memset( input_str, 0, 30*sizeof(char) );
	// strcat(input_str, "0");
	for(k=0;k<strlen(input_str_)-1;k++)
		input_str[k] = input_str_[k];
	// strcat(input_str, "0");
	
	printk(KERN_INFO "INSIDE apply_encryption FUNCTION\n");
	printk(KERN_INFO "INPUT STR:  %s\n", input_str);
	int i, j;

	// int size_input_str=19;			
	int size_input_str=strlen(input_str);			

	
	int draft = get_order_of_key();	
	printk(KERN_INFO "PERMUT_KEY_SIZE :  %d\n", PERMUT_KEY_SIZE);
	// PERMUT_KEY_SIZE
	// PERMUT_KEY
	// ORDER_KEY
	
	// int size_total_str = sizeof(input_str)/sizeof(input_str[0]);
	printk(KERN_INFO "size_input_str :  %d\n", size_input_str);
	
	
	int size_blocks = size_input_str/PERMUT_KEY_SIZE;		//	THIS SHOULD BE ROUND UP(IF FLOAT)
	//Rounding Up if not divided properly, then update the string
	if(size_blocks*PERMUT_KEY_SIZE != size_input_str){
		int difference = (size_blocks+1)*PERMUT_KEY_SIZE - size_input_str;
		printk(KERN_INFO "difference : %d\n", difference);
		
		for(k=0; k<difference;k++)
			strcat(input_str, "0");
		
		size_input_str = size_input_str + difference;		//update size
		size_blocks = size_blocks +	 1;						//update block size		
	}
	


	printk(KERN_INFO "Updated string :  %s\n", input_str);
	
	printk(KERN_INFO "size_input_str_UPDATED :  %d\n", size_input_str);
	printk(KERN_INFO "size_blocks :  %d\n", size_blocks);
	
	char encrypted_str[size_input_str+1]; // = {'\0'};						//returning encrypted string, same size with input str
	memset( encrypted_str, 0, size_input_str*sizeof(char) );
	char INPUT_STR_BLOCKS[size_blocks][PERMUT_KEY_SIZE];	//input str is parsed into blocks for encryption	
	char ENCRYPTED_STR_BLOCKS[size_blocks][PERMUT_KEY_SIZE];
	for(i=0;i<size_blocks;i++){	
		for(j=0;j<PERMUT_KEY_SIZE;j++){	
			INPUT_STR_BLOCKS[i][j] = input_str[(i*PERMUT_KEY_SIZE)+j];
			// printk(KERN_INFO "i %d, j %d,input_str[(i*PERMUT_KEY_SIZE)+j]  %c\n", i, j, input_str[(i*PERMUT_KEY_SIZE)+j]);
		}	
	}

	// for(i=0;i<size_blocks;i++){	
		// for(j=0;j<PERMUT_KEY_SIZE;j++){	
			// printk(KERN_INFO "i %d, j %d, INPUT_STR_BLOCKS[i][j]  %c\n", i, j, INPUT_STR_BLOCKS[i][j]);
		// }	
	// }	
	
	
	// PYTHON CODE EXAMPLE ALGORITHM
	// for i in range(0, len(input_str)):
		// new_str += input_str[ORDER_KEY[i]-1]
	// print(new_str)

	char temp_str[PERMUT_KEY_SIZE];
	memset( temp_str, 0, strlen(temp_str)*sizeof(char) );
	// char *temp_str = kmalloc(PERMUT_KEY_SIZE * sizeof(char), GFP_KERNEL);
    // memset(temp_str, 0, PERMUT_KEY_SIZE * sizeof(char));
	for(i=0;i<size_blocks;i++){	
		for(j=0; j<PERMUT_KEY_SIZE; j++){	
			// encrypted_str[(i*PERMUT_KEY_SIZE)+j] = INPUT_STR_BLOCKS[i][ORDER_KEY[j]-1];
			ENCRYPTED_STR_BLOCKS[i][j] = INPUT_STR_BLOCKS[i][ORDER_KEY[j]-1];
			temp_str[j] = ENCRYPTED_STR_BLOCKS[i][j];
		}

		strncpy(temp_str, ENCRYPTED_STR_BLOCKS[i], PERMUT_KEY_SIZE);
		printk(KERN_INFO "temp_str :  %s\n",temp_str);
		strcat(encrypted_str, temp_str);//, PERMUT_KEY_SIZE); 
	}
	
	// strcat(draft_str, ENCRYPTED_STR_BLOCKS[0]);
	// printk(KERN_INFO "draft_str  %s\n", draft_str);
	// for(i=0;i<size_input_str;i++){	
		// encrypted_str[i] = input_str[ORDER_KEY[i]-1];
		
	// }
	
	printk(KERN_INFO "ENCRYPTED STR:  %s\n", encrypted_str);
	strncpy(returned_encrypted_str, encrypted_str, size_input_str);;
	
	// apply_decryption(encrypted_str);
	
	// strncpy(returned_encrypted_str, encrypted_str, strlen(input_str));
	// return 0;
	// return encrypted_str;
}

/*
	DECRYPT THE GIVEN STRING AND RETURN THE VALUE
*/
// int apply_decryption(char* input_str){
// void apply_decryption(char* input_str){		//worked
void apply_decryption(char* input_str, char* returned_encrypted_str){
// void apply_decryption(char* input_str_){
	
	int draft = get_order_of_key();	
	int i,j;
	// int  arr_size = sizeof(ORDER_KEY)/sizeof(ORDER_KEY[0]);
	int  arr_size = PERMUT_KEY_SIZE;
	int REVERSE_ORDER_KEY[PERMUT_KEY_SIZE];
	int ORDINAL_PERMUT_KEY[PERMUT_KEY_SIZE];
	for(i=0;i<arr_size; i++)
		ORDINAL_PERMUT_KEY[i] = i+1;
	
	for(i=0;i<arr_size; i++)
	REVERSE_ORDER_KEY[ORDER_KEY[i]-1] = ORDINAL_PERMUT_KEY[i] ;
	// for(i=0;i<arr_size; i++){
		// // printk(KERN_INFO "ORDER_KEY[i] = %d\n", ORDER_KEY[i]);
		// printk(KERN_INFO "REVERSE_ORDER_KEY[i] = %d\n", REVERSE_ORDER_KEY[i]);
	// }
	
	
	// // DECRYPT USING REVERSE_ORDER_KEY
	int size_input_str=strlen(input_str);
	int size_blocks = size_input_str/PERMUT_KEY_SIZE;		//	THIS SHOULD BE ROUND UP(IF FLOAT)

	char decrypted_str[size_input_str+1]; 						//returning encrypted string, same size with input str
	memset( decrypted_str, 0, size_input_str*sizeof(char) );
	char INPUT_STR_BLOCKS[size_blocks][PERMUT_KEY_SIZE];	//input str is parsed into blocks for encryption	
	char DECRYPTED_STR_BLOCKS[size_blocks][PERMUT_KEY_SIZE];
	for(i=0;i<size_blocks;i++){	
		for(j=0;j<PERMUT_KEY_SIZE;j++){	
			INPUT_STR_BLOCKS[i][j] = input_str[(i*PERMUT_KEY_SIZE)+j];
			// printk(KERN_INFO "i %d, j %d,input_str[(i*PERMUT_KEY_SIZE)+j]  %c\n", i, j, input_str[(i*PERMUT_KEY_SIZE)+j]);
		}	
	}


	// char temp_str[PERMUT_KEY_SIZE];
	// memset( temp_str, 0, strlen(temp_str)*sizeof(char) );
	char *temp_str = kmalloc(PERMUT_KEY_SIZE * sizeof(char), GFP_KERNEL);
    memset(temp_str, 0, PERMUT_KEY_SIZE * sizeof(char));
	for(i=0;i<size_blocks;i++){	
		for(j=0; j<PERMUT_KEY_SIZE; j++){	
			// encrypted_str[(i*PERMUT_KEY_SIZE)+j] = INPUT_STR_BLOCKS[i][ORDER_KEY[j]-1];
			DECRYPTED_STR_BLOCKS[i][j] = INPUT_STR_BLOCKS[i][REVERSE_ORDER_KEY[j]-1];
			temp_str[j] = DECRYPTED_STR_BLOCKS[i][j];
		}

		strncpy(temp_str, DECRYPTED_STR_BLOCKS[i], PERMUT_KEY_SIZE);
		printk(KERN_INFO "temp_str :  %s\n",temp_str);
		strcat(decrypted_str, temp_str);//, PERMUT_KEY_SIZE); 
	}

	
	//trim decrypted string
	char *decrypted_str_trimmed = kmalloc(40 * sizeof(char), GFP_KERNEL);
    memset(decrypted_str_trimmed, 0, 40 * sizeof(char));	
	for(i=0; i<strlen(decrypted_str); i++){
		if(decrypted_str[i] != '0')
			// printk(KERN_INFO "__DEBUG HERE:  %c\n", decrypted_str[i]);
			decrypted_str_trimmed[i] = decrypted_str[i];
	}
	
	
	
	printk(KERN_INFO "__DECRYPTED STR:  %s\n", decrypted_str);
	printk(KERN_INFO "__DECRYPTED STR_TRIMMED:  %s\n", decrypted_str_trimmed);
	// strncpy(returned_encrypted_str, decrypted_str, size_input_str);
	strncpy(returned_encrypted_str, decrypted_str_trimmed, size_input_str);
	
	
	return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos)
{
    struct scull_dev *dev = filp->private_data;
    int quantum = dev->quantum;
    int s_pos, q_pos;
    ssize_t retval = 0;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

	// do not permit to read if not written
	printk(KERN_INFO "READ: dev->data_written %d\n", dev->data_written);
	if(dev->data_written == 0){
        retval = -EPERM;
		// printk(KERN_INFO "READ: dev->data_written %d\n", dev->data_written);
        goto out;
    }	
	
    if (*f_pos >= dev->size)
        goto out;
    if (*f_pos + count > dev->size)
        count = dev->size - *f_pos;

    s_pos = (long) *f_pos / quantum;
    q_pos = (long) *f_pos % quantum;

    if (dev->data == NULL || ! dev->data[s_pos])
        goto out;

    /* read only up to the end of this quantum */
    if (count > quantum - q_pos)
        count = quantum - q_pos;
	/*
		APPLY DECRYPTION FIRST, THAN COPY TO USER SPACE
	*/
    // if (copy_to_user(buf, dev->data[s_pos] + q_pos, count)) {
	apply_decryption((char*)dev->data[s_pos] + q_pos, (char*)dev->data[s_pos] + q_pos);
    if (copy_to_user(buf, dev->data[s_pos] + q_pos, count)) {
        retval = -EFAULT;
        goto out;
    }

	// apply_decryption((char*)dev->data[s_pos] + q_pos, (char*)dev->data[s_pos] + q_pos);
	printk(KERN_INFO "READ: dev->data[s_pos] + q_pos :  %s\n", dev->data[s_pos] + q_pos);
	printk(KERN_INFO "READ: Decrypted with PERMUT_KEY  %s\n", PERMUT_KEY);
    *f_pos += count;
    retval = count;
	
	if(dev->data_written == 1){
		dev->data_written = 0;
		scull_trim(dev);
	}
		
	// data_written = 0;
  out:
    up(&dev->sem);
    return retval;
}




ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos)
{
	
	/*
		ENCRYPT THE DATA FIRST AND THEN STORE TO THE BUFFER
		
	*/
	
	
    struct scull_dev *dev = filp->private_data;
    int quantum = dev->quantum, qset = dev->qset;
    int s_pos, q_pos;
    ssize_t retval = -ENOMEM;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
	
	if(dev->data_written == 1){
        retval = -EPERM;
        goto out;
    }	 

    if (*f_pos >= quantum * qset) {
        retval = 0;
        goto out;
    }

    s_pos = (long) *f_pos / quantum;
    q_pos = (long) *f_pos % quantum;

    if (!dev->data) {
        dev->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
        if (!dev->data)
            goto out;
        memset(dev->data, 0, qset * sizeof(char *));
    }
    if (!dev->data[s_pos]) {
        dev->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
        if (!dev->data[s_pos])
            goto out;
    }
    /* write only up to the end of this quantum */
    if (count > quantum - q_pos)
        count = quantum - q_pos;

	
	/*
		PROPER WAY TO CREATE AND INITIALIZE STRING
	*/
    // char *my_local_buffer = kmalloc((count + 50) * sizeof(char), GFP_KERNEL);
	
    // memset(my_local_buffer, 0, (count + 50) * sizeof(char));
	
	
	// char *mybuf = apply_encryption(buf);
	// printk(KERN_INFO "INSIDE WRITE :  %s\n", mybuf);
    if (copy_from_user(dev->data[s_pos] + q_pos, buf, count)) {
    // if (copy_from_user(dev->data[s_pos] + q_pos, apply_encryption(buf), count)) {
        retval = -EFAULT;
        goto out;
    }
	
	
	// strcpy(dev->data[s_pos] + q_pos, apply_encryption(dev->data[s_pos] + q_pos));
	apply_encryption((char*)dev->data[s_pos] + q_pos, (char*)dev->data[s_pos] + q_pos);
	printk(KERN_INFO "dev->data[s_pos] + q_pos :  %s\n", dev->data[s_pos] + q_pos);
	// dev->data[s_pos] + q_pos = apply_encryption(dev->data[s_pos] + q_pos);
    *f_pos += count;
    retval = count;

    /* update the size */
    if (dev->size < *f_pos)
        dev->size = *f_pos;
	
	
	dev->data_written = 1;
	
  out:
    up(&dev->sem);
    return retval;
}



long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)			// UPDATE FOR NEW KERNEL
// int scull_ioctl(struct inode *inode, struct file *filp,
                // unsigned int cmd, unsigned long arg)
{

	int err = 0, tmp;
	int retval = 0;

	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		// err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));							// UPDATE FOR NEW KERNEL
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		// err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
		err =  !access_ok( (void __user *)arg, _IOC_SIZE(cmd));			// UPDATE FOR NEW KERNEL
	if (err) return -EFAULT;

	switch(cmd) {
	  case SCULL_IOCRESET:
		scull_quantum = SCULL_QUANTUM;
		scull_qset = SCULL_QSET;
		break;

	  case SCULL_IOCSQUANTUM: /* Set: arg points to the value */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		retval = __get_user(scull_quantum, (int __user *)arg);
		break;

	  case SCULL_IOCTQUANTUM: /* Tell: arg is the value */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		scull_quantum = arg;
		break;

	  case SCULL_IOCGQUANTUM: /* Get: arg is pointer to result */
		retval = __put_user(scull_quantum, (int __user *)arg);
		break;

	  case SCULL_IOCQQUANTUM: /* Query: return it (it's positive) */
		return scull_quantum;

	  case SCULL_IOCXQUANTUM: /* eXchange: use arg as pointer */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_quantum;
		retval = __get_user(scull_quantum, (int __user *)arg);
		if (retval == 0)
			retval = __put_user(tmp, (int __user *)arg);
		break;

	  case SCULL_IOCHQUANTUM: /* sHift: like Tell + Query */
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_quantum;
		scull_quantum = arg;
		return tmp;

	  case SCULL_IOCSQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		retval = __get_user(scull_qset, (int __user *)arg);
		break;

	  case SCULL_IOCTQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		scull_qset = arg;
		break;

	  case SCULL_IOCGQSET:
		retval = __put_user(scull_qset, (int __user *)arg);
		break;

	  case SCULL_IOCQQSET:
		return scull_qset;

	  case SCULL_IOCXQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_qset;
		retval = __get_user(scull_qset, (int __user *)arg);
		if (retval == 0)
			retval = put_user(tmp, (int __user *)arg);
		break;

	  case SCULL_IOCHQSET:
		if (! capable (CAP_SYS_ADMIN))
			return -EPERM;
		tmp = scull_qset;
		scull_qset = arg;
		return tmp;
		
		
		
		/*	// // // // // // // // // // // // // // // // // //// // // // // // // // // // // // /  
			MY CODE IS ADDED AFTER HERE
		*/// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
		
		case CHANGE_KEY:
				copy_from_user(PERMUT_KEY ,(char*) arg, sizeof(PERMUT_KEY));
				printk(KERN_INFO "PERMUT_KEY is written. Value = %s\n", PERMUT_KEY);
				// printk(KERN_INFO "MY FUNCTIONS VALUE IS %d\n", myfunction(5, 4));
				break;
		case CLEAR_CONTENT:
				// copy_to_user((char*) arg, PERMUT_KEY, sizeof(PERMUT_KEY));
				printk(KERN_INFO "CLEAR_CONTENT called \n");
				copy_from_user((char*) arg, draft_string_, sizeof(draft_string_));
				dev_for_trim = filp->private_data;
				scull_trim(dev_for_trim);
				printk(KERN_INFO "IOCTL_TRIMMED \n");
				break;			

		case GET_KEY:
				copy_to_user((char*) arg, PERMUT_KEY, sizeof(PERMUT_KEY));
				printk(KERN_INFO "GET_KEY called . Value = %s\n", PERMUT_KEY);
				printk(KERN_INFO "GET_KEY LEN IS %d\n", strlen(PERMUT_KEY));
				break;
				
		
		
	  default:  /* redundant, as cmd was checked against MAXNR */
		return -ENOTTY;
	}
	return retval;
}


loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
    struct scull_dev *dev = filp->private_data;
    loff_t newpos;

    switch(whence) {
        case 0: /* SEEK_SET */
            newpos = off;
            break;

        case 1: /* SEEK_CUR */
            newpos = filp->f_pos + off;
            break;

        case 2: /* SEEK_END */
            newpos = dev->size + off;
            break;

        default: /* can't happen */
            return -EINVAL;
    }
    if (newpos < 0)
        return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}


struct file_operations scull_fops = {
    .owner =    THIS_MODULE,
    .llseek =   scull_llseek,
    .read =     scull_read,
    .write =    scull_write,
    // .ioctl =    scull_ioctl,
    .unlocked_ioctl =   scull_ioctl,		// UPDATE FOR NEW KERNEL
    .open =     scull_open,
    .release =  scull_release,
};


void scull_cleanup_module(void)
{
    int i;
    dev_t devno = MKDEV(scull_major, scull_minor);

    if (scull_devices) {
        for (i = 0; i < scull_nr_devs; i++) {
            scull_trim(scull_devices + i);
            cdev_del(&scull_devices[i].cdev);
        }
    kfree(scull_devices);
    }
	printk(KERN_WARNING "SCULL MODULE HAS BEEN CLEANED_UP\n");
    unregister_chrdev_region(devno, scull_nr_devs);
}


int scull_init_module(void)
{
    int result, i;
    int err;
    dev_t devno = 0;
    struct scull_dev *dev;

    if (scull_major) {
        devno = MKDEV(scull_major, scull_minor);
        result = register_chrdev_region(devno, scull_nr_devs, "scull");
    } else {
        result = alloc_chrdev_region(&devno, scull_minor, scull_nr_devs,
                                     "scull");
        scull_major = MAJOR(devno);
    }
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }

    scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev),
                            GFP_KERNEL);
    if (!scull_devices) {
        result = -ENOMEM;
        goto fail;
    }
    memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

    /* Initialize each device. */
    for (i = 0; i < scull_nr_devs; i++) {
        dev = &scull_devices[i];
        dev->quantum = scull_quantum;
        dev->qset = scull_qset;
        // init_MUTEX(&dev->sem);				// UPDATE FOR NEW KERNEL
        sema_init(&dev->sem, 1);

        devno = MKDEV(scull_major, scull_minor + i);
        cdev_init(&dev->cdev, &scull_fops);
        dev->cdev.owner = THIS_MODULE;
        dev->cdev.ops = &scull_fops;
        err = cdev_add(&dev->cdev, devno, 1);
        if (err)
            printk(KERN_NOTICE "Error %d adding scull%d", err, i);
    }
	
	printk(KERN_WARNING "SCULL MODULE HAS BEEN INSTALLED\n");		// MY DEBUG PRINT
	// int draft = get_order_of_key();								// FOR DEBUG PURPOSE
	// char INPUT_STR[20] ="oneringtorulethemall";
	// int draft = apply_encryption(INPUT_STR);						// FOR DEBUG PURPOSE


	/*
		MY CODE HERE
	*/
	//initialize global values
	printk(KERN_WARNING "_Initialazing global values.\n");
	// char PERMUT_KEY[20] = "abcd";	
	// char draft_string_[20] = "zzzz";

	// char *PERMUT_KEY;
	// char *draft_string_; 
	PERMUT_KEY = kmalloc(30 * sizeof(char), GFP_KERNEL);
	memset(PERMUT_KEY, 0, 30 * sizeof(char));
	draft_string_ = kmalloc(30 * sizeof(char), GFP_KERNEL);
	memset(draft_string_, 0, 30 * sizeof(char));
	strcat(PERMUT_KEY, "abcd");
	strcat(draft_string_, "zzzz");
	printk(KERN_WARNING "_Initialazed: %s, %s.\n", PERMUT_KEY, draft_string_);
	
	
    return 0; /* succeed */

  fail:
    scull_cleanup_module();
    return result;
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
