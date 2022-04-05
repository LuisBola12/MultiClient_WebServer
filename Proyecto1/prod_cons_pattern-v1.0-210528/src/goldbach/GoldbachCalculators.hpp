// Copyright 2021 Mariana Murillo, Jarod Venegas, Luis Alberto Bolanos
#ifndef CALCULATORS
#define CALCULATORS
#include "../prodcons/Consumer.hpp"
#include "Structs.hpp"
class GoldbachCalculators : Consumer<std::pair<std::pair<int, int>, int64_t>> {
  DISABLE_COPY(GoldbachCalculators);

 public:
  explicit GoldbachCalculators(
      Queue<std::pair<std::pair<int, int>, int64_t>>* queue_of_requests,
      std::vector<In_vector*>* response_vector);
  ~GoldbachCalculators();
  void finish_thread();

 protected:
  // std::vector<std::pair<Semaphore*, Response>>* response_vector;
  std::vector<In_vector*>* vector_for_producers;
  bool verify_prime(int64_t number);
  bool verify_pair(int64_t number);
  void setProducingVector();
  void do_goldbach_sums();
  void even_goldbach_sums(Data& data);
  void init_memory(Response& response);
  void odd_goldbach_sums(Data& data);
  int run() override;
  void consume(const std::pair<std::pair<int, int>, int64_t>& data) override;
};
#endif
