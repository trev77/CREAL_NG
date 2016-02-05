#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <err.h>

void catchinstance(int, siginfo_t*, void*);
void catchhup();
void catchpipe();
void catchterm();
void catchchld();

