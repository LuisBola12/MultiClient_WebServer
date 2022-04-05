// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0
// Serial web server's initial code for parallelization
/**
 * @file main.c
 * @author Mariana Murillo Quintana(mariana.murilloquintana@ucr.ac.cr)
 * @author Jarod Venegas Alpizar(jarod.venegas@ucr.ac.cr)
 * @author Luis Alberto Bolanos Valverde(luis.bolanosvalverde@ucr.ac.cr)
 * @brief  Servidor concurrente que calcula las sumas de goldbach
 * @version 0.1
 * @date 2021-06-13
 * @copyright Copyright (c) 2021
 *
 */

#ifdef WEBSERVER
#include <signal.h>

#include "WebServer.hpp"
static void signal_handler(int numSignal) {
  switch (numSignal) {
    case SIGINT:
      // printf("\nRECEIVED SIGINT\n");
      // std::cout << "\nRECEIVED SIGINT\n" << std::endl;
      WebServer::getInstance()->stopCondition();
      break;
    case SIGTERM:
      // printf("\nRECEIVED SIGTERM\n");
      // std::cout << "\nRECEIVED SIGTERM\n" << std::endl;
      WebServer::getInstance()->stopCondition();
      break;
  }
}
int main(int argc, char *argv[]) {
  if (signal(SIGINT, signal_handler) == SIG_ERR)
    fprintf(stderr, "Could not register signal handler for SIGINT\n");
  if (signal(SIGTERM, signal_handler) == SIG_ERR)
    fprintf(stderr, "Could not register signal handler for SIGTERM\n");
  WebServer::getInstance()->start(argc, argv);
  delete WebServer::getInstance();
  return 0;
}

#endif  // WEBSERVER
