// Copyright 2021 Mariana Murillo, Jarod Venegas, Luis Alberto Bolanos
#ifndef GOLDBACHWEBAPP
#define GOLDBACHWEBAPP
#include <string>

#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include "GoldbachCalculators.hpp"
class GoldbachWebApp {
 public:
  GoldbachWebApp();
  ~GoldbachWebApp();
  bool createHomepageResponse(HttpRequest& httpRequest,
                              HttpResponse& httpResponse);
  bool serveGoldbachSums(HttpRequest& httpRequest, HttpResponse& httpResponse,
                         std::string numbers);
  void stop_condition();

 private:
  // GoldbachManager* manager;
  int current_id;
  std::mutex mutex;
  int current_max_connections;
  int max_of_calculators;
  std::vector<GoldbachCalculators*> calculators;
  int64_t* create_vec_of_numbers(std::string parse_route,
                                 std::string& rejected_values,
                                 int64_t& number_of_values);
  bool is_valid(int64_t num);
  Queue<std::pair<std::pair<int, int>, int64_t>>* queue_of_consuming;

  std::vector<In_vector*>* vector_for_producers;
  int get_current_id();
  std::string send_to_produce(int64_t* numbers, int64_t counter);
  void init_memory_of_producers();
  std::string toString(Data* data_of_request, int64_t counter);
};
#endif
