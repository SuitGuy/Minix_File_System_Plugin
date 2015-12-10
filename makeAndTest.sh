make;
insmod minix.ko;

dd if=/dev/zero of=./disk bs=1024 count=4096;
mkfs.minix ./disk;
mkdir ./minixdir;
mount -o loop ./disk ./minixdir;

touch ./minixdir/testfile1.txt;
touch ./minixdir/testfile4.txt;
touch ./minixdir/testfile3.txt;
touch ./minixdir/testfile2.txt;


