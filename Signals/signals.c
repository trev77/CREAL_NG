#include "libraries.h"

void catchinstance(int signo, siginfo_t *info, void *context)
{
  printf("Error [%d]. ", signo);

  if(signo == 1)
  {
    printf("SIGHUP: Terminal hung up.\n");
    exit(1);
  }
  if(signo == 13)//should never get to this since we ignore
    printf("SIGPIPE: Broken pipe. Ignoring.\n"); 
  
  if(signo == 15)
  {
    printf("SIGTERM: Killing process.\n");
    exit(1);
  }
  if(signo == 17)
  {
    printf("SIGCHLD: Waiting on all dead children.\n");
    while(waitpid(-1, NULL, WNOHANG) > 0){}
  }
}

void catchhup()
{
  static struct sigaction act;

  act.sa_sigaction = &catchinstance;
  act.sa_flags = SA_SIGINFO;
  sigfillset(&(act.sa_mask));
  sigaction(SIGHUP, &act, NULL);
 }


void catchpipe()
{
  static struct sigaction act;
 
  act.sa_handler = SIG_IGN;
  sigfillset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGPIPE, &act, NULL);
}

void catchterm()
{
  static struct sigaction act;

  act.sa_sigaction = &catchinstance;
  sigfillset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGTERM, &act, NULL);
}


void catchchld()
{
  static struct sigaction act;
 
  act.sa_sigaction = &catchinstance;
  sigfillset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGCHLD, &act, NULL);
}

/*
int main()
{
  static struct sigaction act;

  act.sa_sigaction = &catchinstance;
  act.sa_flags = SA_SIGINFO;
  sigfillset(&(act.sa_mask));

  sigaction(SIGHUP, &act, NULL);//1
  sigaction(SIGKILL, &act, NULL);//9
  sigaction(SIGTERM, &act, NULL);//15
  sigaction(SIGCHLD, &act, NULL);//17

  act.sa_handler = SIG_IGN;
  sigfillset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGPIPE, &act, NULL);//13

  int choice, i;

  printf("Test: \n");
  printf("1. SIGHUP\n");
  printf("2. SIGPIPE\n");
  printf("3. SIGKILL\n");
  printf("4. SIGTERM\n");
  printf("5. SIGCHLD\n");
  printf("Choice: \n");
  scanf("%d", &choice);

  if(choice != 5)
  {
    while(1)
    {
      printf("Looping with pid %d. Kill me if you can.\n", getpid());
      sleep(1);
    }
  }

  if(choice == 5)
  {
    for (i = 0; i < 50; i++) 
    {
      //cout << "forking" << endl;
      switch (fork()) 
      {
        case -1:
          perror ("fork");
          return 1;
        case 0:
          return 0;
      }
    }
  }
  return 0;  
}
*/

