#include <kernel/fcntl.h>
#include <kernel/types.h>
#include <user/user.h>

char buffer[512];

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(2, "No arguments for head.\n");
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
        fprintf(2, "Open operation failed.\n");
        exit(1);
    }

    int n;
    int i = 0;
    int j = 0;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        while(i < count && j < n)
        {
            if(buffer[j] == '\n')
            {
                i++;
            }
            j++;
        }
    }

    buffer[j] = '\0';
    printf("%s", buffer);
    close(fd);
    return 0;
}