#include "market_stream/market_stream_printer.h"

namespace market_stream {

MarketStreamPrinter::MarketStreamPrinter(
    const std::weak_ptr<EventHubHandler>& event_handler, std::ostream& out_stream)
    : out_stream_(out_stream) {
  SUBSCRIBE_TO_EVENT(event_handler, &MarketStreamPrinter::OnMarketStreamEvent);
}

MarketStreamPrinter::~MarketStreamPrinter() { std::flush(out_stream_); }

void MarketStreamPrinter::OnMarketStreamEvent(MQ::Event event, const void* data) {
  switch (event) {
    case MQ::Event::kNewTradeEvent:
      out_stream_ << *static_cast<const market_stream::types::Trade*>(data);
      break;

    case MQ::Event::kOrderBookUpdateEvent:
      out_stream_ << *static_cast<const market_stream::types::OrderBook*>(data);
      break;
    default:
      spdlog::error("MarketStreamPrinter unkown event {}", static_cast<int>(event));
      break;
  }
}

}  // namespace market_stream