#include "scsp_queue.hpp"
#include <cassert>
#include <iostream>
#include <pthread.h>

void *write_test(void *queue) {
    int stop = 500;
    for (int idx = 0; idx < stop; ++idx)
        ((SCSPQueue *)queue)->enqueue(rand());

    pthread_exit(NULL);
}

void *read_test(void *queue) {
    int stop = 500;
    for (int idx = 0; idx < stop; ++idx)
        ((SCSPQueue *)queue)->dequeue();

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    SCSPQueue queue;

    pthread_t writer_thread_1, writer_thread_2, writer_thread_3;
    pthread_t reader_thread_1, reader_thread_2, reader_thread_3;

    pthread_create(&writer_thread_1, NULL, write_test, &queue);
    pthread_create(&writer_thread_2, NULL, write_test, &queue);

    pthread_join(writer_thread_1, NULL);
    pthread_join(writer_thread_2, NULL);

    std::cout << "Тесты записи пройдены" << std::endl;

    pthread_create(&reader_thread_1, NULL, read_test, &queue);
    pthread_create(&reader_thread_2, NULL, read_test, &queue);

    pthread_join(reader_thread_1, NULL);
    pthread_join(reader_thread_2, NULL);

    assert(queue.empty());

    std::cout << "Тесты чтения пройдены" << std::endl;

    pthread_create(&writer_thread_3, NULL, write_test, &queue);
    pthread_create(&reader_thread_3, NULL, read_test, &queue);

    pthread_join(writer_thread_3, NULL);
    pthread_join(reader_thread_3, NULL);

    std::cout << "Тесты параллельной записи и чтения пройдены" << std::endl;

    return 0;
}
