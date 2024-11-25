#include "signals_handling.h"
#include "commands.h"
#include "signal.h"
void signal_handler(int sig)
{
    if (foreground_pid != -1)
    {
        kill(foreground_pid, sig);
    }
}

void setup_signal_handlers()
{   
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);  // CTRL-C
    sigaction(SIGTSTP, &sa, NULL); // CTRL-Z
    sigaction(SIGQUIT, &sa, NULL); // CTRL-
}
