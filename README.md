# VirtualFileSystem
This is a C program that provides several command-line utilities, such as listing directory contents, changing the current directory, copying and moving files, displaying the current working directory, and importing/exporting environment variables.
The program defines several functions:
list_root_directory takes a directory path as an argument and lists the contents of the directory.
ls takes a directory path as an argument and lists the contents of the directory.
cat takes a filename as an argument and prints the contents of the file to the console.
export takes an environment variable name and value as arguments and sets the environment variable.
cd takes a directory path as an argument and changes the current working directory to that path.
pwd prints the current working directory to the console.
cp takes two file paths as arguments and copies the contents of the source file to the destination file.
mv takes two file paths as arguments and moves the source file to the destination file.
import takes two file paths as arguments and imports the contents of the source file to the destination file.
print takes one or more filenames as arguments and prints the contents of each file to the console.
The program uses several C library functions to interact with the file system and environment variables, including opendir, readdir, fopen, fclose, fread, fwrite, getcwd, chdir, putenv, rename, fgetc, and fputc.



In order to use this program perform the following steps:

1.) Download disk.c to your desktop
2.) open CMD
3.) run cd Desktop
4.) run gcc -o disk disk.c
5.) run disk
6.) The code should be running

Testing:
1.) In order to test that it works first try running "disk pwd" it should give you a file path
2.) Try creating a text file onto your desktop and write something inside of the text file, then run disk cat text.txt (replace text.txt with respective file name)
the expected result should print whatever was written in the file.
