// Copyright 2021 Mariana Murillo, Jarod Venegas, Luis Alberto Bolanos
#ifndef STRUCTS
#define STRUCTS
#include "stdint.h"
/**
 * @brief Struct for the sums of the number
 *
 */
typedef struct {
  int64_t first;
  int64_t second;
  int64_t third;
} Map;

/**
 * @brief Struct for a number and all the information it needs
 *
 */
typedef struct {
  int64_t number;
  bool sign;
  int64_t position;
  int64_t sums_of_number;
  Map *map;
} Data;

/**
 * @brief Struct for all the numbers of a request
 *
 */
typedef struct {
  Data *vector_of_numbers;
  int64_t counter;
  std::mutex mutex;
} Response;
/**
 * @brief Struct with a semaphore for the threads
 *
 */
typedef struct {
  Semaphore *can_get_my_request;
  Response response;
} In_vector;

#endif
