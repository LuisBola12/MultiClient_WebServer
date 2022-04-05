// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0
#include "WebServer.hpp"

#include <cassert>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

#include "NetworkAddress.hpp"
#include "Socket.hpp"
const char* const usage =
    "Usage: webserv [port] [max_connections]\n"
    "\n"
    "  port             Network port to listen incoming HTTP requests, "
    "default " DEFAULT_PORT
    "\n"
    "  max_connections  Maximum number of allowed client connections\n";
/**
 *@brief Constructor of a Singleton class, also inits a binary
 *semaphore to prevent race conditions, also creates an object
 *for GoldbachWebApp
 */
WebServer* WebServer::serverInstance = 0;
WebServer::WebServer() : access_regex(1) {
  this->goldbachWeb = new GoldbachWebApp();
}
/**
 *@brief If the serverInstance hasn't been initialized, it
 *initializes it, otherwise it returns the instance of the server
 *@return serverInstance
 */
WebServer* WebServer::getInstance() {
  if (serverInstance == 0) {
    serverInstance = new WebServer();
  }
  return serverInstance;
}
/**
 *@brief Destructor of Webserver, it destroys the instances, also deletes the
 * the objects of the server
 */
WebServer::~WebServer() {
  // delete this->goldbachWeb;
  // delete this->connectionQueue;
}
/**
 *@brief Starts the server, by first analizing the arguments introduced at
 * at the main, then create the number of httpConnectionHandler set by
 * the user, then it starts to listen and accept all the connections, once
 * the user sends a Signal, the server will Stop and waits for the threads
 * to finish.
 *@param int argc number of arguments sent by the user
 *@param char* argv[] store of the arguments
 *@return EXIT_SUCCESS
 */
int WebServer::start(int argc, char* argv[]) {
  try {
    if (this->analyzeArguments(argc, argv)) {
      this->connectionHandler.resize(number_of_threads);
      this->connectionQueue = new Queue<Socket>;
      for (int index = 0; index < number_of_threads; index++) {
        connectionHandler[index] =
            new HttpConnectionHandler(connectionQueue, this);
      }
      this->listenForConnections(this->port);
      const NetworkAddress& address = this->getNetworkAddress();
      std::cout << "web server listening on " << address.getIP() << " port "
                << address.getPort() << "...\n";
      this->acceptAllConnections();
    }
  } catch (const std::runtime_error& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  for (int index = 0; index < number_of_threads; index++) {
    Socket null_socket;
    this->connectionQueue->push(null_socket);
    // std::cout << "ENDING PROCESS" << std::endl;
  }
  for (int64_t index = 0; index < number_of_threads; index++) {
    connectionHandler[index]->finish_thread();
    delete (connectionHandler[index]);
  }
  // delete &this->connectionHandler;
  delete this->goldbachWeb;
  delete this->connectionQueue;
  return EXIT_SUCCESS;
}
/**
 *@brief receives the signal sent by th user and proceeds to close
 *the server and stop the threads
 */
void WebServer::stopCondition() {
  stopListening();
  this->goldbachWeb->stop_condition();
}
/**
 *@brief analyze the arguments in order to set the port, and number of
 * threds.
 *@param int argc number of arguments sent by the user
 *@param char* argv[] store of the arguments
 */
bool WebServer::analyzeArguments(int argc, char* argv[]) {
  // Traverse all arguments
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    if (argument.find("help") != std::string::npos) {
      std::cout << usage;
      return false;
    }
  }

  if (argc >= 2) {
    this->port = argv[1];
  }
  if (argc >= 3) {
    if (sscanf(argv[2], "%zu", &this->number_of_threads) < 1 ||
        this->number_of_threads == 0) {
      std::cout << "invalid number of threads" << std::endl;
      number_of_threads = 10;
    }
  } else {
    number_of_threads = 10;  /// NUMBER OF THREADS BY DEFAULT
  }

  return true;
}
/**
 *@brief Handdles the request of the user and generates a response
 *based on what the user asked for.
 *@param HttpRequest request of the user
 *@param HttpResponse response to the user
 *@return bool expression
 */
bool WebServer::handleHttpRequest(HttpRequest& httpRequest,
                                  HttpResponse& httpResponse) {
  // Print IP and port from client
  // WARNING: THIS CAN CAUSE A POSSIBLE RACE CONDITION
  /*
  const NetworkAddress& address = httpRequest.getNetworkAddress();
  std::cout << "connection established with client " << address.getIP()
            << " port " << address.getPort() << std::endl;
  */

  // Print HTTP request
  // WARNING: THIS CAN CAUSE A POSSIBLE RACE CONDITION
  /*
  std::cout << "  " << httpRequest.getMethod() << ' ' << httpRequest.getURI()
            << ' ' << httpRequest.getHttpVersion() << std::endl;
  */
  return this->route(httpRequest, httpResponse);
}
/**
 *@brief regex the route sends by the user, if the user only requested the
 * homepage returns it.
 *@param HttpRequest request of the user
 *@param HttpResponse response to the user
 *@return bool expression
 */
bool WebServer::route(HttpRequest& httpRequest, HttpResponse& httpResponse) {
  // If the home page was asked
  if (httpRequest.getMethod() == "GET" && httpRequest.getURI() == "/") {
    return this->goldbachWeb->createHomepageResponse(httpRequest, httpResponse);
  }
  std::smatch matches;
  // If a number was asked in the form "/goldbach/1223"
  // or "/goldbach?number=1223"
  this->access_regex.wait();
  std::regex inQuery("^/goldbach(/|\\?number=)(.*)$");
  if (std::regex_search(httpRequest.getURI(), matches, inQuery)) {
    assert(matches.length() >= 3);
    const std::string numbers = matches[2];
    this->access_regex.signal();
    // std::cout << "\n (((((( ENTRA EN EL GOLDBACH ))))))\n" << std::endl;
    return this->goldbachWeb->serveGoldbachSums(httpRequest, httpResponse,
                                                numbers);
    // std::cout << "\n(((((( sALE DEL GOLDBACH ))))))\n" << std::endl;
  }
  this->access_regex.signal();
  // Unrecognized request
  return this->serveNotFound(httpRequest, httpResponse);
}
/**
 *@brief Creates an Html response in the case that the server is not found
 *@param HttpRequest request of the user
 *@param HttpResponse response to the user
 *@return bool expression
 */
bool WebServer::serveNotFound(HttpRequest& httpRequest,
                              HttpResponse& httpResponse) {
  (void)httpRequest;

  // Set HTTP response metadata (headers)
  httpResponse.setStatusCode(404);
  httpResponse.setHeader("Server", "AttoServer v1.0");
  httpResponse.setHeader("Content-type", "text/html; charset=ascii");

  // Build the body of the response
  std::string title = "Not found";
  httpResponse.body()
      << "<!DOCTYPE html>\n"
      << "<html lang=\"en\">\n"
      << "  <meta charset=\"ascii\"/>\n"
      << "  <title>" << title << "</title>\n"
      << "  <style>body {font-family: monospace} h1 {color: red}</style>\n"
      << "  <h1>" << title << "</h1>\n"
      << "  <p>The requested resouce was not found in this server.</p>\n"
      << "  <hr><p><a href=\"/\">Homepage</a></p>\n"
      << "</html>\n";

  // Send the response to the client (user agent)
  return httpResponse.send();
}
