#include <kernel/types.h>
#include <user/user.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(2, "No arguments for mgrep.\n");
        exit(1);
    }

    for (int i = 2; i < argc; i++)
    {
        char* input[] = {"grep", argv[1] ,argv[i], 0};
        printf("\nmgrep %d \n", i - 1);
        int pid = fork();
        if (pid < 0)
        {
            fprintf(2, "fork no:%d failed.\n", i - 1);
        }
        if (pid)
        {
            int status;
            pid = wait(&status);
            printf(".........Lines........\n");
            printf("Status %d.\n", status);
        }
        else
        {
            printf("Child process no:%d \n", getpid());
            printf(".........Lines........\n");
            exec(input[0], input);
            fprintf(2, "exec no:%d failed.\n", i - 1);
        }
    }
}