#ifndef INCLUDE_MARKET_STREAM_SAVED_MARKET_STREAM_FORWARDER_H_
#define INCLUDE_MARKET_STREAM_SAVED_MARKET_STREAM_FORWARDER_H_

#include <atomic>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <functional>
#include <memory>
#include <string>

#include "analyzer/benchmark_orchestrator.h"
#include "events/event_hub.h"
#include "utils/time/types.h"

namespace market_stream {
class SavedMarketStreamForwarder {
  using MQ = events::message_queues::MarketStream;
  using EventHubDispatcher = events::EventHub<MQ>::Dispatcher;

 public:
  SavedMarketStreamForwarder() = delete;
  SavedMarketStreamForwarder(const SavedMarketStreamForwarder &) = delete;
  SavedMarketStreamForwarder(SavedMarketStreamForwarder &&) = delete;
  SavedMarketStreamForwarder &operator=(const SavedMarketStreamForwarder &) = delete;
  SavedMarketStreamForwarder &operator=(SavedMarketStreamForwarder &&) = delete;

  SavedMarketStreamForwarder(
      const std::string &saved_file_path,
      const std::weak_ptr<EventHubDispatcher> &event_dispatcher,
      const std::shared_ptr<analyzer::SavedStreamForwarderUnitState> &unit_state =
          nullptr);
  virtual ~SavedMarketStreamForwarder();

  virtual void Initialize();
  virtual bool ReadNext(utils::Timestamp *next_data_ts = nullptr);
  virtual void ForwardNext();

 private:
  template <class T>
  bool ReadNext(T &data) {
    try {
      (*archive_) >> data;
    } catch (const boost::archive::archive_exception &e) {
      spdlog::error("Error during deserialization: {}", e.what());
      return false;
    }
    return true;
  }

  std::ifstream file_;
  boost::filesystem::path file_path_;
  std::unique_ptr<boost::archive::binary_iarchive> archive_;
  std::function<void()> forward_next_func_;
  std::weak_ptr<EventHubDispatcher> event_dispatcher_;
  std::shared_ptr<analyzer::SavedStreamForwarderUnitState> unit_state_;
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_SAVED_MARKET_STREAM_FORWARDER_H_