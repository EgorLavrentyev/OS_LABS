#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>

#include <iostream>

#include "daemon.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Usage: ./daemon config.cfg" << endl;
        exit(EXIT_FAILURE);
    }

    openlog("daemon", LOG_CONS | LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Opened a connection to the system logger.");
    syslog(LOG_INFO, "Run.");

    Daemon &dmn = Daemon::GetInstance(argv[1]);

    pid_t pid = fork();

    if (pid == -1)
    {
        syslog(LOG_ERR, "First fork error.");
        exit(EXIT_FAILURE);
    }

    if (pid != 0)
        exit(EXIT_SUCCESS);

    if (setsid() == (pid_t)(-1))
    {
        syslog(LOG_ERR, "Setsid error.");
        exit(EXIT_FAILURE);
    }

    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, Daemon::HandleSignal);
    signal(SIGTERM, Daemon::HandleSignal);

    Daemon::ProtectAgainstRestart();

    while (true)
    {
        dmn.DeleteAllTmp();
        sleep(dmn.GetInterval());
    }
}
