/* This comes from http://www.developerweb.net/forum/showthread.php?t=2990
   and was found by Cratylus (http://anyterm.org/forums/viewtopic.php?pid=4503#p4503).
   The author appears to be Alexandre Jasmin.  There is no copyright or license
   text associated with this, but it was published in a public forum and it
   might be reasonable to assume that the author did not intend to prevent its
   re-use.  */

#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/ioctl.h> 
#include <sys/stream.h> 
#include <sys/stropts.h> 

/* fork_pty() remplacement for Solaris.
 * This ignore the last two arguments
 * for the moment
 */
int
forkpty (int *amaster,
            char *name,
            void *unused1,
            void *unused2)
{
  int master, slave;
  char *slave_name;
  pid_t pid;

  master = open("/dev/ptmx", O_RDWR);
  if (master < 0)
    return -1;

  if (grantpt (master) < 0)
    {
      close (master);
      return -1;
    }

  if (unlockpt (master) < 0)
    {
      close (master);
      return -1;
    }

  slave_name = ptsname (master);
  if (slave_name == NULL)
    {
      close (master);
      return -1;
    }

  slave = open (slave_name, O_RDWR);
  if (slave < 0)
    {
      close (master);
      return -1;
    }

  if (ioctl (slave, I_PUSH, "ptem") < 0
      || ioctl (slave, I_PUSH, "ldterm") < 0)
    {
      close (slave);
      close (master);
      return -1;
    }


  if (amaster)
    *amaster = master;

  if (name)
    strcpy (name, slave_name);

  pid = fork ();
  switch (pid)
    {
    case -1: /* Error */
      return -1;
    case 0: /* Child */
      close (master);
      dup2 (slave, STDIN_FILENO);
      dup2 (slave, STDOUT_FILENO);
      dup2 (slave, STDERR_FILENO);
      return 0;
    default: /* Parent */
      close (slave);
      return pid;
    }

  return -1;
}


