/**
 *  @file ndsignal.c
 *  @brief Program Signal Implementation
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#include "ndeployblink1/core/ndsignal.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* Exit flag */
int g_endprog = 0;

/* Signal object */
static struct sigaction act;

void nd_signal_init()
{
    /* Use the sa_sigaction field */
    act.sa_sigaction = &nd_signal_end;

    /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field */
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        fprintf(stderr, "sigaction SIGINT failed to handled");
        exit(1);
    }

    if (sigaction(SIGTSTP, &act, NULL) < 0) {
        fprintf(stderr, "sigaction SIGTSTP failed to handled");
        exit(1);
    }
}

void nd_signal_end()
{
    g_endprog = 1;
}
