#include <cstring>
#include <iostream>
#include <pthread.h>
#include <stdexcept>
#include <sys/syslog.h>
#include <sys/un.h>
#include <unistd.h>

#include "conn.hpp"

#include <sys/socket.h>

class ConnSocket : public Conn {
  public:
    ConnSocket(const std::string &name, bool create) : isHost(create), name(name) {
        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;

        std::strncpy(addr.sun_path, name.c_str(), sizeof(addr.sun_path) - 1);

        if (isHost) {
            if ((hostSocket = socket(AF_UNIX, SOCK_SEQPACKET, 0)) < 0) {
                throw std::runtime_error("Can't create host socket");
            }

            if (bind(hostSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                throw std::runtime_error("Can't bind host socket");
            }

            if ((listen(hostSocket, MAX_CLIENT_NUM)) < 0) {
                throw std::runtime_error("Can't listen connection");
            }

            pthread_create(&accept_thread, NULL, ConnSocket::accept_socket, this);
        } else {
            if ((clientSocket = socket(AF_UNIX, SOCK_SEQPACKET, 0)) < 0) {
                throw std::runtime_error("Can't create client socket");
            }

            if (connect(clientSocket, (const struct sockaddr *)&addr, sizeof(sockaddr_un)) < 0) {
                throw std::runtime_error("Can't connect sockets");
            }
        }
    }

    ~ConnSocket() {
        if (isHost) {
            if (::close(clientSocket) < 0) {
                std::cout << "Failed to close client socket" << std::endl;
                std::exit(1);
            }
            if (::close(hostSocket) < 0) {
                std::cout << "Failed to close host socket" << std::endl;
                std::exit(1);
            }
            if (unlink(name.c_str()) < 0) {
                std::cout << "Failed to unlink socket" << std::endl;
                std::exit(1);
            }
        } else {
            if (::close(clientSocket) < 0) {
                std::cout << "Failed to close client socket" << std::endl;
                std::exit(1);
            }
        }
    }

    void read(Message &msg) override {
        if (isHost)
            pthread_join(accept_thread, NULL);

        if (recv(clientSocket, &msg, sizeof(Message), 0) < 0) {
            throw std::runtime_error("Failed to read from socket");
        }
    }
    void write(const Message &msg) override {
        if (isHost)
            pthread_join(accept_thread, NULL);

        if (send(clientSocket, &msg, sizeof(Message), MSG_NOSIGNAL) < 0) {
            throw std::runtime_error("Failed to write to socket");
        }
    }

  private:
    static void *accept_socket(void *conn) {
        if ((((ConnSocket *)conn)->clientSocket = accept(((ConnSocket *)conn)->hostSocket, NULL, NULL)) < 0) {
            throw std::runtime_error("Can't accept client socket");
        }

        pthread_exit(NULL);
    }

    static constexpr size_t MAX_CLIENT_NUM = 1;

    socklen_t hostSocket;
    socklen_t clientSocket;

    pthread_t accept_thread;

    bool isHost = false;
    std::string name;
};

Conn *Conn::create(bool create, int conn_id) {
    std::string name = "/tmp/wolf_n_goat_socket_conn_" + std::to_string(conn_id);
    return new ConnSocket(name, create);
}
