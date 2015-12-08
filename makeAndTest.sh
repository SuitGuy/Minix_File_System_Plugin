make;
insmod minix.ko;

dd if=/dev/zero of=./disk bs=1024 count=4096;
mkfs.minix ./disk;
mkdir ./minixdir;
mount -o loop ./disk ./minixdir;



