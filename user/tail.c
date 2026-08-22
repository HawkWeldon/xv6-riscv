#include <kernel/fcntl.h>
#include <kernel/types.h>
#include <user/user.h>

char buffer[1024];

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

    int n = read(fd, buffer, sizeof(buffer));
    if (n < 0)
    {
        fprintf(2, "Read operation failed.\n");
        exit(1);
    }

    int i = 0;
    int j = n;
    while(i < (count + 1) && j > 0)
    {
        if(buffer[j] == '\n')
        {
            i++;
        }
        j--;
    }

    printf("%s",&buffer[j+2]);
    return 0;
}