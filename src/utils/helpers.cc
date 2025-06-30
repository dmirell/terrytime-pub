#include "utils/helpers.h"

#include <spdlog/spdlog.h>

#include <cstdio>
#include <thread>

namespace utils {

void RunSyncTimerWithLogging(const std::string& timer_prefix,
                             const std::chrono::duration<int>& sleep_duration) {
  // Start time
  auto start_time = std::chrono::high_resolution_clock::now();

  // Logging the time remaining
  while (true) {
    // Update the time remaining
    auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
    auto remaining_time = sleep_duration - elapsed_time;

    if (remaining_time.count() <= 0) {
      break;  // Exit the loop when sleep is complete
    }

    // Print the time remaining with escape sequences for clearing lines
    auto hours = std::chrono::duration_cast<std::chrono::hours>(remaining_time);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(
        remaining_time % std::chrono::hours(1));
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
        remaining_time % std::chrono::minutes(1));

    std::string timer_message = "\r" + timer_prefix + " %.2d:%.2d:%.2d";

    /*printf(timer_message.c_str(), hours.count(), minutes.count(),
           seconds.count());
    fflush(stdout);*/

    // Sleep for a short interval (adjust as needed)
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  printf("\n");
}

bool IsDoublesEqual(market_stream::types::DoubleType a,
                    market_stream::types::DoubleType b,
                    market_stream::types::DoubleType eps) {
  const bool normal_comparation = (a == b);
  const bool epsilon_comparation = (a - eps <= b && b <= a + eps);
  if (normal_comparation != epsilon_comparation) {
    spdlog::error("Test numbers: {}, {}, Normal comparation: {}, Epsilon comparation: {}",
                  a.str(), b.str(), normal_comparation, epsilon_comparation);
    exit(-1);
  }
  return epsilon_comparation;
}
}  // namespace utils