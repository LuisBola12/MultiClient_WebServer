// Copyright 2021 Mariana Murillo, Jarod Venegas, Luis Alberto Bolanos
#include "GoldbachCalculators.hpp"

#include <iostream>
/**
 * @brief Construct a new Goldbach Calculators:: Goldbach Calculators object
 *
 * @param queue_of_requests queue of requests
 * @param response_vector vector of the esponses of each request
 */
GoldbachCalculators::GoldbachCalculators(
    Queue<std::pair<std::pair<int, int>, int64_t>>* queue_of_requests,
    std::vector<In_vector*>* response_vector) {
  setConsumingQueue(queue_of_requests);
  this->vector_for_producers = response_vector;
  this->startThread();
}
/**
 * @brief Destroy the Goldbach Calculators:: Goldbach Calculators object
 *
 */
GoldbachCalculators::~GoldbachCalculators() {}
/**
 * @brief Determine if a number is prime or not
 *
 * @param number the number to verify
 * @return true if it is prime
 * @return false it it is not prime
 */
bool GoldbachCalculators::verify_prime(int64_t number) {
  if (number == 2 || number == 3 || number == 5 || number == 7) {
    return true;
  }
  if (number % 2 == 0 || number % 3 == 0 || number % 5 == 0 ||
      number % 7 == 0) {
    return false;
  } else {
    for (int64_t i = 11; i * i <= number; i += 6) {
      if (number % i == 0 || number % (i + 2) == 0) {
        return false;
      }
    }
    return true;
  }
}
/**
 * @brief Determine if a number is even or odd
 *
 * @param number the number to verify
 * @return true if it is even
 * @return false if it is odd
 */
bool GoldbachCalculators::verify_pair(int64_t number) {
  if (number % 2 == 0) {
    return true;
  }
  return false;
}
/**
 * @brief Calculate the number of goldbach sums for even numbers
 *
 * @param data struct data
 */
void GoldbachCalculators::even_goldbach_sums(Data& data) {
  int64_t counter = 0;
  int64_t num = data.number;
  if (num < 0) {
    num = num * -1;
  }
  int64_t num2 = 0;
  int64_t size = 50;
  for (int64_t index1 = 2; index1 <= num / 2; index1++) {
    if (verify_prime(index1)) {
      num2 = num - index1;
      if (num2 >= index1 && verify_prime(num2)) {
        if (data.sign) {
          if (counter == size) {
            size = size * 2;
            data.map = (Map*)realloc(data.map, size * sizeof(Map));
          }
          data.map[counter].first = (int64_t)index1;
          data.map[counter].second = (int64_t)num2;
        }
        counter++;
      }
    }
  }
  data.sums_of_number = counter;
}
/**
 * @brief Calculate the number of goldbach sums for odd numbers
 *
 * @param data struct data
 */
void GoldbachCalculators::odd_goldbach_sums(Data& data) {
  int64_t counter = 0;
  int64_t size = 50;
  int64_t num = data.number;
  if (num < 0) {
    num = num * -1;
  }
  int64_t num2 = 0;
  int64_t max_iterations = (num + 1) / 2;
  for (int64_t index1 = 2; index1 <= max_iterations; index1++) {
    if (verify_prime(index1)) {
      for (int64_t index2 = index1; index2 <= (num / 2); index2 += 2) {
        if (verify_prime(index2)) {
          num2 = num - index1;
          num2 = num2 - index2;
          if (num2 >= index2 && verify_prime(num2)) {
            if (data.sign) {
              if (counter == size) {
                size = size * 2;
                data.map = (Map*)realloc(data.map, size * sizeof(Map));
              }
              data.map[counter].first = (int64_t)index1;
              data.map[counter].second = (int64_t)index2;
              data.map[counter].third = (int64_t)num2;
            }
            counter++;
          }
        }
      }
    }
  }
  data.sums_of_number = counter;
}
/**
 * @brief Virtual method from the grandfather class: Thread
 *
 * @return int exit_success
 */
int GoldbachCalculators::run() {
  this->consumeForever();
  return EXIT_SUCCESS;
}
/**
 * @brief Virtual method from the father class: Consumer. It cosumes the numbers
 * from the queue of the threads and calculate the sums of that number. It has
 * a mutex to controls the signal of the thread waiting in the GoldbachWebApp
 * class.
 *
 * @param data struct data
 */
void GoldbachCalculators::consume(
    const std::pair<std::pair<int, int>, int64_t>& data) {
  // Pair.first.first = response_id, first.second = index on vec_request
  //.second = number
  std::pair<std::pair<int, int>, int64_t> numbers = data;
  Data data_of_number;
  int response_id = numbers.first.first;
  int pos_on_vec = numbers.first.second;
  data_of_number.number = numbers.second;
  data_of_number.map = (Map*)calloc(50, sizeof(Map));
  if (data_of_number.number < 0) {
    data_of_number.sign = true;
  } else {
    data_of_number.sign = false;
  }
  if (verify_pair(data_of_number.number)) {
    even_goldbach_sums(data_of_number);
  } else {
    odd_goldbach_sums(data_of_number);
  }
  vector_for_producers[0][response_id]->response.mutex.lock();
  if (vector_for_producers[0][response_id]->response.counter == 1) {
    vector_for_producers[0][response_id]
        ->response.vector_of_numbers[pos_on_vec] = data_of_number;
    vector_for_producers[0][response_id]->response.counter--;
    vector_for_producers[0][response_id]->response.mutex.unlock();
    vector_for_producers[0][response_id]->can_get_my_request->signal();
  } else {
    vector_for_producers[0][response_id]->response.counter--;
    vector_for_producers[0][response_id]
        ->response.vector_of_numbers[pos_on_vec] = data_of_number;
    vector_for_producers[0][response_id]->response.mutex.unlock();
  }
}
/**
 * @brief Virtual methid from grandfather class. Makes a join to the thread.
 *
 */
void GoldbachCalculators::finish_thread() { this->waitToFinish(); }
