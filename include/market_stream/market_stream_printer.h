#ifndef INCLUDE_MARKET_STREAM_MARKET_STREAM_PRINTER_H_
#define INCLUDE_MARKET_STREAM_MARKET_STREAM_PRINTER_H_

#include <memory>
#include <ostream>

#include "events/event_hub.h"
#include "market_stream/types/types.h"

namespace market_stream {

class MarketStreamEventDispatcher;

class MarketStreamPrinter {
  using MQ = events::message_queues::MarketStream;
  using EventHubHandler = events::EventHub<MQ>::Handler;

 public:
  MarketStreamPrinter() = delete;
  MarketStreamPrinter(const MarketStreamPrinter &) = delete;
  MarketStreamPrinter(MarketStreamPrinter &&) = delete;
  MarketStreamPrinter &operator=(const MarketStreamPrinter &) = delete;
  MarketStreamPrinter &operator=(MarketStreamPrinter &&) = delete;

  MarketStreamPrinter(const std::weak_ptr<EventHubHandler> &event_handler,
                      std::ostream &out_stream);
  ~MarketStreamPrinter();

 private:
  void OnMarketStreamEvent(MQ::Event event, const void *data);

  std::ostream &out_stream_;
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_MARKET_STREAM_PRINTER_H_