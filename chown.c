#include <linux/proc_fs.h>
#include <linux/dcache.h>
#include <linux/kernel.h> 
#include <linux/namei.h>
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
	__u32 sum =0;
	u32 bits;
	
	sbi = minix_sb(sb);
	s_imap = sbi->s_imap;
	u32 bits = sbi->s_ninodes + 1;
	unsigned blocks = DIV_ROUND_UP(bits, sb->s_blocksize * 8);
	
	while (blocks--) {
		unsigned words = blocksize / 2;
		__u16 *p = (__u16 *)(*map++)->b_data;
		while (words--)
			sum += 16 - hweight16(*p++);
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

	if(ino >0){
		minix_chown(sb, ino ,path, uid);
	}else{
		process_inodes(sb, uid){
	}
	path_put(&path);	
	/*else{
		process_inodes(sb, uid); 
	}
*/
	return 0;
	
	

}
