// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#include "HttpServer.hpp"

#include <cassert>
#include <stdexcept>
#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Socket.hpp"
HttpServer::HttpServer() {}
HttpServer::~HttpServer() {}

void HttpServer::listenForever(const char* port) {
    return TcpServer::listenForever(port);
}

void HttpServer::handleClientConnection(Socket& client) {
    Socket connection = client;
    connectionQueue->push(connection);
}
