// Copyright 2021 Mariana Murillo, Jarod Venegas, Luis Alberto Bolanos
#include "HttpConnectionHandler.hpp"

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "unistd.h"
/**
 * @brief Construct a HttpConnectionHandler object.
 * Works as a thread. As many instances as necessary are created (threads).
 * 
 * @param connectionQueue Queue containing the sockets.
 * @param server Reference to the server for calling public
 * subroutines of that object.
 */
HttpConnectionHandler::HttpConnectionHandler(Queue<Socket>* connectionQueue,
                                             HttpServer* server) {
    this->setConsumingQueue(connectionQueue);
    reference_to_server = server;
    this->startThread();
}
/**
 * @brief Calls waitToFinish() (Thread´s subroutine).
 * 
 */
void HttpConnectionHandler::finish_thread() { this->waitToFinish(); }
/**
 * @brief Overrided subroutine of class Thread (subroutine performed by thread)
 * 
 * @return int EXIT_SUCCESS
 */
int HttpConnectionHandler::run() {
    this->consumeForever();
    return EXIT_SUCCESS;
}
/**
 * @brief Overrided subrutine to process any data extracted from the queue.
 * 
 * @param data 
 */
void HttpConnectionHandler::consume(const Socket& data) {
    Socket client = data;
    while (true) {
        // Create an object that parses the HTTP request from the socket
        HttpRequest httpRequest(client);
        // If the request is not valid or an error happened
        if (!httpRequest.parse()) {
            // Non-valid requests are normal after a previous valid request. Do
            // not close the connection yet, because the valid request may take
            // time to be processed. Just stop waiting for more requests
            break;
        }

        // A complete HTTP client request was received. Create an object for the
        // server responds to that client's request
        HttpResponse httpResponse(client);

        // Give subclass a chance to respond the HTTP request
        const bool handled =
            reference_to_server->handleHttpRequest(httpRequest, httpResponse);

        // If subclass did not handle the request or the client used HTTP/1.0
        if (!handled || httpRequest.getHttpVersion() == "HTTP/1.0") {
            // The socket will not be more used, close the connection
            client.close();
            break;
        }

        // This version handles just one client request per connection
    }
}
