#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int running = 1;

void handler(int sig, siginfo_t* info, void* ucontext)
{
    int value = info->si_value.sival_int;
    int pid = info->si_pid;

    if (value == 0) {
        running = 0;
        return;
    }
    --value;

    sigval_t sigval;
    sigval.sival_int = value;
    sigqueue(pid, SIGRTMIN, sigval);
}

int main(int argc, char** argv)
{
    sigset_t sigset;
    sigfillset(&sigset);
    sigdelset(&sigset, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    struct sigaction action = {.sa_sigaction = &handler,
                               .sa_flags = SA_RESTART | SA_SIGINFO};
    sigaction(SIGRTMIN, &action, NULL);

    while (running) {
        pause();
    }

    return 0;
}

