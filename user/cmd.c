#include <kernel/types.h>
#include <user/user.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("missing args \n");
        return 1;
    }
    int n = fork ();
    if (n < 0) 
    {
        printf("fork failed.\n");
        exit(1);
    }
    if (!n) 
    {
        exec(argv[1], &argv[1]);
        printf("exec failed.\n");
        exit(1);
    }
    else
    {
        wait(0);
        printf("\n");
    }
    return 0;
}