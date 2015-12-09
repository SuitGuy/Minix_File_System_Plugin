#ifndef CHOWN_H_   /* Include guard */
#define CHOWN_H_

/*change the ownership for one inode*/
 void minix_chown(struct inode * i_node,int ino ,struct path path,int uid);

/*change the ownership for all inodes in sb*/
int process_inodes(struct super_block *sb, int uid);

/*handles which of the chown functions above to call based on the ino value*/
int proc_chown(int ino, char * filepath, int uid);



#endif // CHOWN_H_
