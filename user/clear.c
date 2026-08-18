#include "kernel/types.h"
#include "user/user.h"
int main()
{
    printf("\033[H\033[J");
    return 0;
}