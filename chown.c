#include <linux/dcache.h>
#include <linux/kernel.h> 
#include <linux/namei.h>
#include <linux/buffer_head.h>
#include <linux/types.h>
#include <linux/string.h>
#include "minix.h"


/* minix_chown: 

sets uid-field for given inode to specified value

*/

int minix_chown( int ino,char* filepath, int uid){
	struct inode * i_node;
	struct dentry *dentry;
	struct path path;
	struct super_block *sb;
	int res;

	//do not alter the root inode
	if(ino == 0 ){
		return 0;
	}

	printk(KERN_INFO "MINIX CHOWN CALLED WITH ino=%i , uid= %i\n", ino , uid);

	//aquire the dentry and super block from the path
	res = kern_path(filepath, LOOKUP_FOLLOW, &path);
	if(res){
		printk(KERN_INFO "Kern Path did not return\n");
		return -EFAULT;
	}
	dentry = path.dentry;
	sb = dentry->d_sb;
	printk(KERN_INFO "checking for minix file system '%s'\n", sb->s_type->name);
	//check that you are operating in a minix filesystem
	
	
	if(strncmp("minix",sb->s_type->name, 5) != 0){
		printk(KERN_INFO "ERROR: \n       Path not in a minix filesystem\n");
		return -EFAULT;
	}

	// aquire inode
	i_node = minix_iget(sb, ino);

	//change user id of inode
	i_node -> i_uid.val = uid;

	//trigger VFS to flush to disk.
	mark_inode_dirty(i_node);

	//return inode to VFS
	iput(i_node);
	
	//return the path to the VFS
	path_put(&path);

	printk(KERN_INFO "CHOWN SUCCESS");
	return 0;
}

/* process_inodes: 
   
  goes through the map of valid inodes and for each valid inode calls minix_chown

 */


int process_inodes(char * filepath, int uid){
	struct dentry *dentry;
	struct path path;
	struct super_block *sb;
	struct minix_sb_info *sbi;
	struct buffer_head ** s_imap;
	int index = 0;
	int comparisonint =1;
	int curino = 0;
	int inodesfound = 0;
	int totalvalidinodes;
	unsigned blocks;
	u32 bits;
	int *p;

	//extract the super block from the path dentry
	if (kern_path(filepath, LOOKUP_FOLLOW, &path)){
		return -EFAULT;
	}
	dentry = path.dentry;
	sb = dentry->d_sb;
	
	//check if it is a minix file system
	if(strncmp("minix",sb->s_type->name, 5) != 0){
		printk(KERN_INFO "ERROR: \n       Path not in a minix filesystem\n");
		return -EFAULT;
	}
	//extract super block info to aquire inode bitmap
	sbi = minix_sb(sb);
	s_imap = sbi->s_imap;
	
	//calculate the number of blocks to check
	bits = sbi->s_ninodes + 1;
	blocks = DIV_ROUND_UP(bits, sb->s_blocksize * 8);
	printk(KERN_INFO "blocksize %lu\n", sb->s_blocksize);
	//calculate the number of valid inodes you should be looking for plus the root 
	totalvalidinodes = sbi->s_ninodes - minix_count_free_inodes(sb) +1;
	printk(KERN_INFO "NO OF VALID INODES %i \n" , totalvalidinodes);

	//for all the blocks you have
	while (blocks-- && (inodesfound < totalvalidinodes)) {

		//words are 2 bytes long so you need to devide the
		//blocksize by 2 to get the number of words in the block.
		unsigned words = sb->s_blocksize / 2;
		p = (int *)(*s_imap++)->b_data;

		//for all the words in the block
		while (words-- && (inodesfound < totalvalidinodes )){
				printk(KERN_INFO "p %i \n", *p);

			//for all of the bits in that word
			while (index < 16 && (inodesfound < totalvalidinodes )){
				printk(KERN_INFO "CURRENT INODE %i\n", curino);
				//do bitwise comparison to check if bit is valid inode				
				if(curino < 16 &&(*p & comparisonint)!= 0){
					printk(KERN_INFO "INODE %i VALID\n", curino);
					
					//call minix_chown on valid inode
					if (minix_chown(curino, filepath, uid) != 0){
						return -EFAULT;
					}
					//found valid inode.
					inodesfound ++;
				}
				printk(KERN_INFO "COMPARISON INT %i\n", comparisonint);
				//left bit shift the comparison int which has the same effect as multiplying it by 2			
				comparisonint = comparisonint << 1;
				
				//increment current inode and index of next bit
				curino ++;
				index++;
			}
			comparisonint = 1;
			//look at the next word in the block.
			p++;
			index = 0;
		}
		
	}

	//return the path to the VFS
	path_put(&path);
	
	return 0;
}

/* proc_chown 

   handles writing to the proc-file. It processes the string passed to it, and calls minix_chown or process_inodes as appropriate.

 */

int proc_chown(int ino, char * filepath, int uid){
	int res;
		
	if(ino > 0){
		//change the ownership for the specified inode
		res = minix_chown(ino,filepath,uid);
	}else if(ino == 0){
		//change the ownership for all inodes in the VFS
		res = process_inodes(filepath, uid);
	}else{
		//invalid inode number supplied.
		return -EFAULT;
	}
	
	return res;
	
	

}
