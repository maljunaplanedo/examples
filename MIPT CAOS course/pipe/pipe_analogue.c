#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>

void solve(char** argv, int number, int parent_pipe)
{
    if (number != 1) {
        int channel[2];
        pipe(channel);

        if (fork() == 0) {
            solve(argv, number - 1, channel[1]);
        }
        close(channel[1]);
        dup2(channel[0], 0);
        wait(NULL);
    }

    if (parent_pipe != -1) {
        dup2(parent_pipe, 1);
    }
    execlp(argv[number], argv[number], NULL);
}

int main(int argc, char** argv)
{
    solve(argv, argc - 1, -1);

    return 0;
}

