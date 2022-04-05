// Copyright 2021 Mariana Murillo, Jarod Venegas, Luis Alberto Bolanos
#include "GoldbachWebApp.hpp"

#include <string>

#include "math.h"
#include "unistd.h"
/**
 * @brief Construct a new Goldbach Web App:: Goldbach Web App object
 *
 */
GoldbachWebApp::GoldbachWebApp() {
  this->max_of_calculators = sysconf(_SC_NPROCESSORS_ONLN);
  this->current_id = 0;
  this->current_max_connections = 20;
  this->queue_of_consuming = new Queue<std::pair<std::pair<int, int>, int64_t>>;
  this->vector_for_producers =
      (std::vector<In_vector*>*)calloc(1, sizeof(std::vector<In_vector>));
  vector_for_producers[0].resize(current_max_connections);
  for (int index = 0; index < current_max_connections; index++) {
    this->vector_for_producers[0][index] =
        (In_vector*)calloc(1, sizeof(In_vector));
  }
  for (int index = 0; index < current_max_connections; index++) {
    this->vector_for_producers[0][index]->can_get_my_request = new Semaphore(0);
  }
  init_memory_of_producers();
}
/**
 * @brief Destroy the Goldbach Web App:: Goldbach Web App object
 *
 */
GoldbachWebApp::~GoldbachWebApp() {}
/**
 * @brief Initializate the memory needed for the program to run
 *
 */
void GoldbachWebApp::init_memory_of_producers() {
  this->calculators.resize(max_of_calculators);
  for (int index = 0; index < this->max_of_calculators; index++) {
    this->calculators[index] = new GoldbachCalculators(
        this->queue_of_consuming, this->vector_for_producers);
  }
}
/**
 * @brief Build the body of the response from the data generated
 * by GoldbachCalculator. Convert the string to a vector of
 * numbers that the algorithm can recognize.
 *
 * @param httpRequest
 * @param httpResponse
 * @param numbers string containing the numbers extracted from the socket
 * @return true on success
 * @return false on error or connection closed
 */
bool GoldbachWebApp::serveGoldbachSums(HttpRequest& httpRequest,
                                       HttpResponse& httpResponse,
                                       std::string numbers) {
  (void)httpRequest;
  // Set HTTP response metadata (headers)
  httpResponse.setHeader("Server", "AttoServer v1.0");
  httpResponse.setHeader("Content-type", "text/html; charset=ascii");
  std::string rejeceted_values;
  int64_t number_of_values;
  int64_t* numbers_to_goldbach =
      create_vec_of_numbers(numbers, rejeceted_values, number_of_values);
  std::string sums_of_goldbach =
      send_to_produce(numbers_to_goldbach, number_of_values);
  // manager->send_vector(numbers_to_goldbach, number_of_values);
  // Build the body of the response
  std::string title = "Goldbach sums ";
  httpResponse.body()
      << "<!DOCTYPE html>\n"
      << "<html lang=\"en\">\n"
      << "<head> "
      << "  <meta charset=\"ascii\"/>\n"
      << "  <title>" << title << "</title>\n"
      << "  <style>body {font-family: monospace} .err {color: red}</style>\n"
      << "</head> "
      << "<body> "
      << "  <h1>" << title << "</h1>\n"
      << sums_of_goldbach << "<h1 class=\"err\">>Rejected values:</h1><BR>"
      << "<span class=\"err\"><h2>" << rejeceted_values << "</h2></span>"
      << "  <hr><p><a href=\"/\">Back</a></p>\n"
      << "</body> "
      << "</html>\n";
  // Send the response to the client (user agent)
  free(numbers_to_goldbach);
  return httpResponse.send();
}
/**
 * @brief It takes the numbers of a request and push them in a queue in form of
 * a pair, then the threads consume from that queue
 *
 * @param vec_in_request
 * @param counter
 * @return std::string
 */
std::string GoldbachWebApp::send_to_produce(int64_t* vec_in_request,
                                            int64_t counter) {
  // std::string output = "Si me imprimo es por que lo logre";
  int response_id = get_current_id();
  // Elements of pair are a pair of the response id and the number in the vector
  // of request Second is the goldbach number
  std::pair<std::pair<int, int>, int64_t> push_pair;
  vector_for_producers[0][response_id]->response.counter = counter;
  vector_for_producers[0][response_id]->response.vector_of_numbers =
      (Data*)calloc(counter, sizeof(Data));
  for (int index = 0; index < counter; index++) {
    push_pair.first.first = response_id;
    push_pair.first.second = index;
    push_pair.second = vec_in_request[index];
    this->queue_of_consuming->push(push_pair);
  }
  vector_for_producers[0][response_id]->can_get_my_request->wait();
  Data* data_per_request =
      vector_for_producers[0][response_id]->response.vector_of_numbers;
  return toString(data_per_request,
                  counter);  // Falta Montar el string de manera correcta
}
/**
 * @brief Takes the requests and makes a string of the sums of each number of
 * the request.
 *
 * @param data_of_request
 * @param counter
 * @return std::string
 */
std::string GoldbachWebApp::toString(Data* data_of_request, int64_t counter) {
  std::string output;
  for (int index = 0; index < counter; index++) {
    if (!data_of_request[index].sign) {
      output += "<h2>" + std::to_string(data_of_request[index].number) +
                "</h2>" + "<p>" +
                std::to_string(data_of_request[index].number) + ": " +
                std::to_string(data_of_request[index].sums_of_number) +
                " sums</p><BR>\n";
    } else {
      output += "<h2> " + std::to_string(data_of_request[index].number) +
                "</h2>" + "<p> " +
                std::to_string(data_of_request[index].number) + ": " +
                std::to_string(data_of_request[index].sums_of_number) +
                " sums</p><BR>\n<ul>";
      for (int index2 = 0; index2 < data_of_request[index].sums_of_number;
           index2++) {
        if (data_of_request[index].number % 2 == 0) {
          output += "<li>" +
                    std::to_string(data_of_request[index].map[index2].first) +
                    " + " +
                    std::to_string(data_of_request[index].map[index2].second) +
                    "</li><BR>";
        } else {
          output +=
              "<li>" +
              std::to_string(data_of_request[index].map[index2].first) + " + " +
              std::to_string(data_of_request[index].map[index2].second) +
              " + " + std::to_string(data_of_request[index].map[index2].third) +
              "</li><BR>";
        }
      }
      output += "</ul>";
    }
    free(data_of_request[index].map);
  }
  free(data_of_request);
  return output;
}
/**
 * @brief Get the id of each thread, the id it is protected with a mutex.
 *
 * @return int
 */
int GoldbachWebApp::get_current_id() {
  int id;
  this->mutex.lock();
  id = this->current_id;
  this->current_id++;
  if (this->current_id == current_max_connections) {
    this->current_max_connections *= 2;
    this->vector_for_producers[0].resize(current_max_connections);
    for (int i = id; i < current_max_connections; i++) {
      this->vector_for_producers[0][i]->can_get_my_request = new Semaphore(0);
    }
  }
  this->mutex.unlock();
  return id;
}
/**
 * @brief Free all the memory that it is needed when the program is stopped.
 *
 */
void GoldbachWebApp::stop_condition() {
  std::pair<std::pair<int, int>, int64_t> stop_condition;
  for (int i = 0; i < max_of_calculators; i++) {
    this->queue_of_consuming->push(stop_condition);
  }
  for (int i = 0; i < max_of_calculators; i++) {
    this->calculators[i]->finish_thread();
    delete this->calculators[i];
  }
  for (int index = 0; index < current_max_connections; index++) {
    delete this->vector_for_producers[0][index]->can_get_my_request;
  }
  for (int i = 0; i < current_max_connections; i++) {
    free(this->vector_for_producers[0][i]);
  }
  delete this->queue_of_consuming;
  free(vector_for_producers);
}
/**
 * @brief Build the home page to be displayed in the browser
 * when it is entered.
 *
 * @param httpRequest
 * @param httpResponse
 * @return true on success.
 * @return false on error.
 */
bool GoldbachWebApp::createHomepageResponse(HttpRequest& httpRequest,
                                            HttpResponse& httpResponse) {
  // Set HTTP response metadata (headers)
  (void)httpRequest;
  httpResponse.setHeader("Server", "AttoServer v1.0");
  httpResponse.setHeader("Content-type", "text/html; charset=ascii");
  // Build the body of the response
  std::string title = "Goldbach sums";
  httpResponse.body() << "<!DOCTYPE html>\n"
                      << "<html lang=\"en\">\n"
                      << "  <meta charset=\"ascii\"/>\n"
                      << "  <title>" << title << "</title>\n"
                      << "  <style>body {font-family: monospace}</style>\n"
                      << "  <h1>" << title << "</h1>\n"
                      << "  <form method=\"get\" action=\"/goldbach\">\n"
                      << "    <label for=\"number\">Number</label>\n"
                      << "    <input type=\"text\" name=\"number\" required/>\n"
                      << "    <button type=\"submit\">Calculate</button>\n"
                      << "  </form>\n"
                      << "</html>\n";
  return httpResponse.send();
}
/**
 * @brief Check the validation of a number
 * @param num number to verify
 * @return true means it is a valid number
 * @return false means it isnt a valid number
 */
bool GoldbachWebApp::is_valid(int64_t num) {
  int64_t limite = (int64_t)pow(2, 63) - 1;
  if ((num < (limite * -1)) || (num > limite) || ((num <= 5) && (num >= -5))) {
    return false;
  }
  return true;
}
/**
 * @brief Parse the string from the regex and make a numbers vector with the
 * numbers of the string
 * @param parse_route  the string from de regex
 * @param rejected_values a string of the values rejected
 * @param counter counter of the numbers vector
 * @return a vector of numbers
 */
int64_t* GoldbachWebApp::create_vec_of_numbers(std::string parse_route,
                                               std::string& rejected_values,
                                               int64_t& counter) {
  int64_t size = 100;
  int64_t* vector_num = (int64_t*)calloc(size, sizeof(int64_t));
  int64_t vector_iterator = 0;
  std::string temp_number_string = "";
  int64_t limit = (int64_t)parse_route.length();
  for (int64_t i = 0; i <= limit; i++) {
    if (parse_route[i] == '%' && parse_route[i + 2] == 'C') {
      temp_number_string += ",";
      i += 2;
    } else {
      if (parse_route[i] == '%' && parse_route[i + 2] == '0') {
        i += 2;
      } else {
        if (parse_route[i] == ',' && parse_route[i] == '%') {
          temp_number_string += ",";
          i += 3;
        } else {
          if (parse_route[i] == ' ' || parse_route[i] == '+') {
            // si se enuentra con un espacio o un + no hace nada
          } else {
            temp_number_string += parse_route[i];
          }
        }
      }
    }
  }

  std::string number_string = "";
  limit = (int64_t)temp_number_string.length();
  for (int64_t i = 0; i <= limit; i++) {
    if (temp_number_string[i] == ',' || i == limit) {
      int64_t number = 0;
      try {
        number = (int64_t)std::stoll(number_string);
        if (is_valid(number)) {
          vector_num[vector_iterator] = number;
          vector_iterator++;
          number_string = "";
        } else {
          rejected_values += number_string + "<br>";
          number_string = "";
        }
      } catch (...) {
        if (isalpha(number_string[0])) {
          rejected_values += number_string + "<br>";
        } else {
          rejected_values += number_string + "<br>";
        }
      }
      number_string = "";
    } else {
      number_string += temp_number_string[i];
    }
  }
  counter = vector_iterator;
  return vector_num;
}
