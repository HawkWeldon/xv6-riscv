#include <kernel/fcntl.h>
#include <kernel/types.h>
#include <user/user.h>

char buffer[512];

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(2, "No arguments for tail.\n");
        exit(1);
    }
    else if (argc == 2)
    {
        fprintf(2, "Please mention number of lines to print.\n");
        exit(1);
    }

    int count = atoi(argv[2]);

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        fprintf(2, "Failed to open the file.\n");
        exit(1);
    }

    int n;
    int i;
    int total_count = 0;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        i = 0;
        while(i < n)
        {
            if (buffer[i] == '\n') total_count++;
            i++;
        }
    }
    close(fd);

    int skip_count = total_count - count;
    if (skip_count < 0) skip_count = 0;

    int current_count = 0;

    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        fprintf(2, "Failed to open the file.\n");
        exit(1);
    }

    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (n < 0)
        {
            fprintf(2, "Failed to read.\n");
            exit(1);
        }
        i = 0;
        while(i < n)
        {
            if (current_count < skip_count) {if(buffer[i] == '\n') current_count++;}
            else write(1, &buffer[i], 1);
            i++;
        }
    }

    close(fd);
    return 0;
}