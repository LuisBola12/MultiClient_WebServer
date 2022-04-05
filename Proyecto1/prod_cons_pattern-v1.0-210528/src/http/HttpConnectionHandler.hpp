// Copyright 2021 Mariana Murillo, Jarod Venegas, Luis Alberto Bolanos
#ifndef CONNECTIONHANDLER_
#define CONNECTIONHANDLER_
#include "../common/common.hpp"
#include "../network/Socket.hpp"
#include "../prodcons/Consumer.hpp"
#include "HttpServer.hpp"
#define CAPACITY 10
class HttpConnectionHandler : Consumer<Socket> {
    DISABLE_COPY(HttpConnectionHandler);

 protected:
    HttpServer* reference_to_server;

 public:
    explicit HttpConnectionHandler(Queue<Socket>* connectionQueue,
                                   HttpServer* server);

    int run() override;

    void finish_thread();

    void consume(const Socket& data) override;
};
#endif
