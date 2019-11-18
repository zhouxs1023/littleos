LittleOS
--------

* What is LittleOS ?
        LittleOS is a new operating system. But LittleOS should not be
the BEST nor the FASTEST (If you want the fastest, take Linux :)),
but LittleOS is made to be very simple to use.

The purpose of this project is to make something stable and simple.
Help is needed ! Everyone could join this project, in order to learn something
new by programming an OS.

* Installing and Compiling:
      - To install LittleOS, just decompress the ZIP archive in a directory,
for example c:\littleos. Then, adjust the variables TOPDIR and DOS_TOPDIR in
c:\littleos\makefile.

      - To compile LittleOS, you'll need GCC, GNU Make and Nasm 0.97+.
Just type: make

* What is supported:
        - there is a console driver (just screen)
        - paging (not complete)
        - irqs (timer, keyboard...)

* What is to do:
        - boot sector compatible with FAT12/16/32
        - paging protections (not virtual memory)
        - floppy & hard drive drivers
        - a file system (ext2? minix? vfat?). The FS should support long
        names.
        - multitasking
        - system calls
        - a better console driver (keyboard+screen+mouse)

Send me your comments and bug reports to: placr@mygale.org (Lacroix Pascal)

Contact me:
web: http://www.mygale.org/~placr/
email: placr@mygale.org
UIN: 7945786
