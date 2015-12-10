#include <linux/proc_fs.h>
#include <linux/dcache.h>
#include <linux/kernel.h> 
#include <linux/namei.h>
#include <linux/buffer_head.h>
#include <linux/types.h>
#include "minix.h"




/* minix_chown: 

sets uid-field for given inode to specified value

*/

void minix_chown(struct super_block * sb, int ino,struct path path, int uid){
	struct inode * i_node;


	// aquire inode
	i_node = minix_iget(sb, ino);

	//change user id of inode
	i_node -> i_uid.val = uid;

	//trigger VFS to flush to disk.
	mark_inode_dirty(i_node);

	//return inode to VFS
	iput(i_node);
	

	return;
}

/* process_inodes: 
   
  goes through the map of valid inodes and for each valid inode calls minix_chown

 */


int process_inodes(struct super_block *sb, int uid){
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

	printk(KERN_INFO "ENTERING PROCESSINODE \n");
	sbi = minix_sb(sb);
	s_imap = sbi->s_imap;
	bits = sbi->s_ninodes + 1;
	blocks = DIV_ROUND_UP(bits, sb->s_blocksize * 8);
	totalvalidinodes = sbi->s_ninodes - minix_count_free_inodes(sb);
	printk(KERN_INFO "NO OF VALID INODES %i \n" , totalvalidinodes);
	while (blocks-- && (inodesfound > totalvalidinodes)) {
		unsigned words = sb->s_blocksize / 2;
		p = (int *)(*s_imap++)->b_data;
		while (words-- && (inodesfound > totalvalidinodes)){
			if(inodesfound > totalvalidinodes){
					break;
				}
				printk(KERN_INFO "p %i \n", *p);

			while (index < 16 && (inodesfound > totalvalidinodes)){
				if(curino < 16 &&(*p & comparisonint)!= 0){
					printk(KERN_INFO "INODE %i VALID\n", curino);
					inodesfound ++;
				}
				printk(KERN_INFO "COMPARISON INT %i\n", comparisonint);
				comparisonint = comparisonint *2 ;
				
				curino ++;
				index++;
			}
			comparisonint = 1;
			p++;
			index = 0;
		}
		if(inodesfound > totalvalidinodes){
			break;
		}
		
	}
	
	return 0;
}

/* proc_chown 

   handles writing to the proc-file. It processes the string passed to it, and calls minix_chown or process_inodes as appropriate.

 */

int proc_chown(int ino, char * filepath, int uid){
	
		

	struct dentry *dentry;
	struct path path;
	struct super_block *sb;

	printk(KERN_INFO "PROC CHOWN %i %i %s", ino, uid, filepath);

	kern_path(filepath, LOOKUP_FOLLOW, &path);
	dentry = path.dentry;
	sb = dentry->d_sb;
	process_inodes(sb, uid);
/*
	if(ino >0){
		minix_chown(sb, ino ,path, uid);
	}else{
		
	}*/
	path_put(&path);	
	/*else{
		process_inodes(sb, uid); 
	}
*/
	return 0;
	
	

}
