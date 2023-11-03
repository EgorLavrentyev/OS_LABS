#ifndef DAEMON_H
#define DAEMON_H

#include <string>

class Daemon
{
public:
    static Daemon& GetInstance(char *cfgName = nullptr);
    static void ProtectAgainstRestart();
    static void HandleSignal(int sigNum);
    const std::string& GetAbsoluteFolderPath();
    int GetInterval();
    void ReadConfig();
    void DeleteAllTmp();

private:
    std::string absPath, absfldrPath;
    int interval;

    Daemon(char *cfgName);
    Daemon(const Daemon&) = delete;
    Daemon& operator=(const Daemon&) = delete;

    static const std::string ABS_PID_PATH;
};

#endif //DAEMON_H
