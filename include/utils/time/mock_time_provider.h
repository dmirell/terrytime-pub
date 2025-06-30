#ifndef INCLUDE_UTILS_TIME_MOCK_TIME_PROVIDER_H_
#define INCLUDE_UTILS_TIME_MOCK_TIME_PROVIDER_H_

#include <condition_variable>
#include <future>
#include <memory>
#include <set>
#include <thread>

#include "utils/time/elapsed_time_meter.h"
#include "utils/time/i_time_provider.h"

namespace utils {

class MockTimeProvider : public ITimeProvider {
 public:
  MockTimeProvider(Timestamp initial_time);

  // ITimeProvider
  virtual void WaitUntil(Timestamp timestamp) override;
  virtual Timestamp Now() const override;

  virtual void JumpToFirstAwaitingTimestamp();
  virtual void JumpToTime(Timestamp ts);

 private:
  void StartFromTime(Timestamp initial_time);

  mutable std::mutex mutex_;
  Timestamp start_time_;
  ElapsedTimeMeter elapsed_time_meter_;
  std::condition_variable cv_;
  std::set<Timestamp> waiting_times_;  // Keeps track of all wake-up times
};

}  // namespace utils

#endif  // INCLUDE_UTILS_TIME_MOCK_TIME_PROVIDER_H_