#include "conn.hpp"
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

class ConnFifo : public Conn {
  public:
    ConnFifo(const std::string &name, bool create) : isHost(create), name(name) {
        if (isHost) {
            if (mkfifo(name.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                throw std::runtime_error("Failed to create");
            }
        }
        descriptor = ::open(name.c_str(), O_RDWR);
        if (descriptor < 0) {
            throw std::runtime_error("Failed to open");
        }
    }

    ~ConnFifo() {
        if (::close(descriptor) < 0) {
            std::cout << "Failed to close" << std::endl;
            std::exit(1);
        }
        if (isHost) {
            if (unlink(name.c_str()) < 0) {
                std::cout << "Failed to unlink" << std::endl;
                std::exit(1);
            }
        }
    }

    void read(Message &msg) override {
        if (::read(descriptor, &msg, sizeof(Message)) < 0) {
            throw std::runtime_error("Failed to read");
        }
    }

    void write(const Message &msg) override {
        if (::write(descriptor, &msg, sizeof(Message)) < 0) {
            throw std::runtime_error("Failed to write");
        }
    }

  private:
    bool isHost = false;
    std::string name;
    int descriptor = -1;
};

Conn *Conn::create(bool create, int conn_id) {
    std::string name = "/tmp/wolf_n_goat_fifo_conn_" + std::to_string(conn_id);
    return new ConnFifo(name, create);
}
