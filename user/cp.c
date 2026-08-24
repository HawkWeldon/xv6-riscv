#include <kernel/fcntl.h>
#include <kernel/types.h>
#include <user/user.h>

char buffer[512];

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(2, "No arguments for cp.\n");
        exit(1);
    }
    else if (argc == 2)
    {
        fprintf(2, "Give a destination.\n");
        exit(1);
    }

    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in < 0)
    {
        fprintf(2, "Failed to open %s.\n", argv[1]);
        exit(1);
    }

    int fd_out = open(argv[2], O_WRONLY | O_TRUNC | O_CREATE);
    if (fd_out < 0)
    {
        fprintf(2, "Failed to open %s.\n", argv[2]);
        exit(1);
    }

    int n;

    while ((n = read(fd_in, buffer, sizeof(buffer))) > 0)
    {
        if (n < 0)
        {
            fprintf(2, "Failed to read.\n");
            exit(1);
        }

        int m = write(fd_out, buffer, sizeof(buffer));
        if (m < 0)
        {
            fprintf(2, "Failed to write.\n");
            exit(1);
        }
    }

    printf("Job Done.\n");
    return 0;
}