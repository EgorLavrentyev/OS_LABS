#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <fstream>

#include "daemon.h"

using namespace std;

const string Daemon::ABS_PID_PATH = "/var/run/daemon.pid";

const string& Daemon::GetAbsoluteFolderPath()
{
    return absfldrPath;
}

int Daemon::GetInterval()
{
    return interval;
}

void Daemon::ReadConfig()
{
    ifstream cfgFile(absPath);
    cfgFile >> absfldrPath >> interval;

    if (cfgFile.eof())
    {
        syslog(LOG_ERR, "Bad config.");
        exit(EXIT_FAILURE);
    }

    cfgFile.close();
    syslog(LOG_INFO, "Read config.");
}

Daemon::Daemon(char *cfgName)
{
    char buf[PATH_MAX];

    if (realpath(cfgName, buf) == nullptr)
    {
        syslog(LOG_ERR, "Config doesn't exist.");
        exit(EXIT_FAILURE);
    }
    Daemon::absPath = buf;
    ReadConfig();
}

void Daemon::ProtectAgainstRestart()
{
    ifstream pidInputFile(ABS_PID_PATH);
    pid_t pidPrevDaemon;
    pidInputFile >> pidPrevDaemon;
    if (!pidInputFile.eof())
        kill(pidPrevDaemon, SIGTERM);
    pidInputFile.close();

    ofstream pidOutputFile(ABS_PID_PATH);
    pidOutputFile << getpid() << endl;
    pidOutputFile.close();
}

void Daemon::HandleSignal(int sigNum)
{
    switch(sigNum)
    {
        case SIGHUP:
        {
            syslog(LOG_INFO, "Catched SIGHUP.");
            Daemon &cfg = Daemon::GetInstance();
            cfg.ReadConfig();
            break;
        }
        case SIGTERM:
            syslog(LOG_INFO, "Catched SIGTERM.");
            syslog(LOG_INFO, "Completed.");
            exit(EXIT_SUCCESS);
        default:
            break;
    }
}

void Daemon::DeleteAllTmp()
{
    //Daemon &dmn = Daemon::GetInstance();
    DIR *dir = opendir(GetAbsoluteFolderPath().c_str());

    if (dir == nullptr)
    {
        syslog(LOG_ERR, "Couldn't open folder.");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_REG)
        {
            string fileName = entry->d_name;
            if (fileName.length() >= 4)
                if (strcmp(".tmp", &(entry->d_name[fileName.length() - 4])) == 0)
                {
                    string filePath = GetAbsoluteFolderPath() + "/" + fileName;
                    if (remove(filePath.c_str()) != 0)
                        syslog(LOG_ERR, "Couldn't remove file %s", filePath.c_str());

                }
        }
    }

    closedir(dir);
}

Daemon& Daemon::GetInstance(char *cfgName)
{
    static Daemon instance(cfgName);
    return instance;
}
