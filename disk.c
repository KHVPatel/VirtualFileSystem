#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// Define the maximum length of a command argument
#define MAX_ARG_LENGTH 256

// Define the maximum number of command arguments
#define MAX_ARGS 64

// Define the maximum length of an environment variable name
#define MAX_VARNAME_LENGTH 256

// Define the maximum length of an environment variable value
#define MAX_VARVALUE_LENGTH 2048

typedef struct partition_entry
{
    unsigned char boot_flag;
    unsigned char start_head;
    unsigned char start_sector;
    unsigned char start_cylinder;
    unsigned char partition_type;
    unsigned char end_head;
    unsigned char end_sector;
    unsigned char end_cylinder;
    unsigned int start_sector_lba;
    unsigned int num_sectors;
} PartitionEntry;

typedef struct mbr
{
    unsigned char bootstrap[446];
    PartitionEntry partition_table[4];
    unsigned short signature;
} MBR;

void list_root_directory(const char *path)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(path);
    if (dir == NULL)
    {
        printf("Failed to open directory\n");
        return;
    }

    printf("Contents of directory %s:\n", path);
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

void ls(char *path)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(path);
    if (dir == NULL)
    {
        fprintf(stderr, "Error: could not open directory %s\n", path);
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

void cat(char *filename)
{
    FILE *file;
    char buffer[MAX_VARVALUE_LENGTH];
    size_t n;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: could not open file %s\n", filename);
        return;
    }

    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        fwrite(buffer, 1, n, stdout);
    }

    fclose(file);
}

void export(char *varname, char *varvalue)
{
    char *envvar;
    size_t envvar_len;

    // Allocate memory for the environment variable string
    envvar_len = strlen(varname) + strlen(varvalue) + 2;
    envvar = (char *)malloc(envvar_len * sizeof(char));

    // Build the environment variable string
    snprintf(envvar, envvar_len, "%s=%s", varname, varvalue);

    // Set the environment variable using putenv
    if (putenv(envvar) != 0)
    {
        fprintf(stderr, "Error: could not set environment variable %s\n", varname);
    }

    // Free the memory allocated for the environment variable string
    free(envvar);
}

void cd(char *dir)
{
    if (chdir(dir) != 0)
    {
        fprintf(stderr, "Error: could not change directory to %s\n", dir);
        return;
    }

    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: could not get current directory\n");
        return;
    }

    printf("Current directory is now: %s\n", cwd);
}


void pwd()
{
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: could not get current directory\n");
    }
    printf("The path is : %s\n", cwd);
}

void cp(const char *source_file, const char *dest_file)
{
    FILE *source, *dest;
    char ch;

    source = fopen(source_file, "r");
    if (source == NULL)
    {
        printf("Unable to open source file.\n");
        return;
    }

    dest = fopen(dest_file, "w");
    if (dest == NULL)
    {
        printf("Unable to open destination file.\n");
        fclose(source);
        return;
    }

    while ((ch = fgetc(source)) != EOF)
    {
        fputc(ch, dest);
    }

    printf("File copied successfully.\n");

    fclose(source);
    fclose(dest);
}

int mv(const char *src_path, const char *dest_path)
{
    if (rename(src_path, dest_path) != 0)
    {
        perror("mv error");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void import(char *source_path, char *destination_path)
{
    FILE *source_file, *destination_file;
    char ch;

    source_file = fopen(source_path, "r");
    if (source_file == NULL)
    {
        printf("Error: Could not open source file.\n");
        return;
    }

    destination_file = fopen(destination_path, "w");
    if (destination_file == NULL)
    {
        printf("Error: Could not open destination file.\n");
        fclose(source_file);
        return;
    }

    while ((ch = fgetc(source_file)) != EOF)
    {
        fputc(ch, destination_file);
    }

    fclose(source_file);
    fclose(destination_file);

    printf("File imported successfully.\n");
}

void print(int argc, char *argv[])
{
    FILE *f = fopen(argv[1], "rb");
    MBR mbr;
    fread(&mbr, sizeof(MBR), 1, f);
    int i = 0;

    for (i; i < 4; i++)
    {
        printf("Partition %d:\n", i + 1);
        printf("  Bootable: %s\n", mbr.partition_table[i].boot_flag == 0x80 ? "yes" : "no");
        printf("  Partition type: %02X\n", mbr.partition_table[i].partition_type);
        printf("  Start sector: %u\n", mbr.partition_table[i].start_sector_lba);
        printf("  Number of sectors: %u\n", mbr.partition_table[i].num_sectors);
    }

    fclose(f);
}

int main(int argc, char *argv[])
{

    FILE *f = fopen(argv[1], "rb");
    MBR mbr;
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s command [arguments...]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "ls") == 0)
    {
        ls(argv[2]);
    }

    else if (strcmp(argv[1], "cat") == 0)
    {
        cat(argv[2]);
    }

    else if (strcmp(argv[1], "export") == 0)
    {
        export(argv[2], argv[3]);
    }

    else if (strcmp(argv[1], "cp") == 0)
    {
        cp(argv[2], argv[3]);
    }

    else if (strcmp(argv[1], "mv") == 0)
    {
        mv(argv[2], argv[3]);
    }

    else if (strcmp(argv[1], "import") == 0)
    {
        import(argv[2], argv[3]);
    }

    else if (strcmp(argv[1], "pwd") == 0)
    {
        pwd();
    }

    else if (strcmp(argv[1], "cd") == 0)
    {
        cd(argv[2]);
    }

    else if (strcmp(argv[1], "list") == 0)
    {
        list_root_directory(argv[2]);
    }

    else if (strcmp(argv[1], "print") == 0)
    {
        print(argc, argv);
    }

    else
    {
        printf("Give a suitable command");        
    }

    fclose(f);
}
