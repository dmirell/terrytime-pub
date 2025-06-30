#ifndef INCLUDE_MARKET_STREAM_MARKET_STREAM_SAVER_H_
#define INCLUDE_MARKET_STREAM_MARKET_STREAM_SAVER_H_

#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <memory>
#include <string>

#include "events/event_hub.h"
#include "market_stream/types/types.h"

namespace market_stream {

class MarketStreamSaver {
  using MQ = events::message_queues::MarketStream;
  using EventHubHandler = events::EventHub<MQ>::Handler;

 public:
  MarketStreamSaver() = delete;
  MarketStreamSaver(const MarketStreamSaver &) = delete;
  MarketStreamSaver(MarketStreamSaver &&) = delete;
  MarketStreamSaver &operator=(const MarketStreamSaver &) = delete;
  MarketStreamSaver &operator=(MarketStreamSaver &&) = delete;

  MarketStreamSaver(const std::weak_ptr<EventHubHandler> &event_handler,
                    const std::string &path);
  ~MarketStreamSaver();

  static std::string filename();

 private:
  void OnMarketStreamEvent(MQ::Event event, const void *data);

  std::ofstream file_;
  boost::filesystem::path file_path_;
  std::unique_ptr<boost::archive::binary_oarchive> archive_;
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_MARKET_STREAM_SAVER_H_