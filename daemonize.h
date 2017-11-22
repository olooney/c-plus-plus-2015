#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>


static void daemonize( void worker(), std::string working_dir="/" )
{
    std::cerr << "called daemonize" << std::endl;
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();
    std::cerr << "first fork pid " << pid << std::endl;

    /* An error occurred */
    /* Success: Let the parent terminate */
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0) exit(EXIT_FAILURE);

    std::cerr << "sedsid " << std::endl;

    /* Catch, ignore and handle signals */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    std::cerr << "ignored SIGCHILD and SIGHUP" << std::endl;

    /* Fork off for the second time*/
    pid = fork();
    std::cerr << "second fork pid " << pid << std::endl;

    /* An error occurred */
    /* Success: Let the parent terminate */
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);
    std::cerr << "set umask(0)" << std::endl;

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    if ( chdir(working_dir.c_str()) ) exit(EXIT_FAILURE);
    std::cerr << "set working directory to " << working_dir << std::endl;

    /* Close all open file descriptors */
    int fd;
    for (fd = sysconf(_SC_OPEN_MAX); fd>=0; fd--)
    {
        close (fd);
        // std::cerr << "closed FD " << fd << std::endl;
    }
    std::cerr << "closed all open file descriptors" << std::endl;
    
    // invoke the worker.
    worker();
    std::cerr << "worker completed; exiting successfully" << std::endl;
    exit(EXIT_SUCCESS);
}

// compared to redis implementation:  https://github.com/antirez/redis/blob/unstable/src/server.c 
/*
there are many differences but also similarities. They ignore SIGHUP and SIGPIPE, but don't
bother with SIGCHILD. They do call setsid().

They also seem to only do a single fork().

Instead of blindly closing all file descriptors, they copy a select few file descriptors
over to /dev/null with dup2:

    int fd;
    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) close(fd);
    }

They don't seem to bother with the umask(0), either. They do support a "dir" config option
and chdir() to it when loading server config.
*/

