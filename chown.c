#include <linux/proc_fs.h>
#include <linux/dcache.h>
#include <linux/kernel.h> 
#include <linux/namei.h>
#include "minix.h"
/* minix_chown: 

sets uid-field for given inode to specified value

*/

void minix_chown(struct super_block * sb, int ino, int uid){
	struct inode * i_node;


	// aquire inode
	i_node = minix_iget(sb, ino);

	//change user id of inode
	i_node -> i_uid.val = uid;

	//return inode to VFS
	iput(i_node);

	//trigger VFS to flush to disk.
	mark_inode_dirty(i_node);

	return;
}

/* process_inodes: 
   
  goes through the map of valid inodes and for each valid inode calls minix_chown

 */


int process_inodes(struct super_block *sb, int uid){
	struct minix_sb_info *sb_info;
	sb_info = minix_sb(sb);

	//get the imap of the inodes and itterate through the nodes setting the uid.
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
		minix_chown(sb, ino , uid);
	}/*else{
		process_inodes(sb, uid); 
	}
*/
	return 0;
	
	

}
