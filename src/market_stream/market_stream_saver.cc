#include "market_stream/market_stream_saver.h"

#include <spdlog/spdlog.h>

namespace market_stream {
MarketStreamSaver::MarketStreamSaver(const std::weak_ptr<EventHubHandler>& event_handler,
                                     const std::string& save_path)
    : file_path_(boost::filesystem::path(save_path) / filename()) {
  spdlog::info("initializing streamsaver.");
  file_.open(file_path_.string(), std::ios::binary | std::ios::out);
  if (!file_.is_open()) {
    spdlog::error("Cannot open the file: {}", file_path_.string());
    exit(EXIT_FAILURE);
  } else {
    spdlog::info("Saving stream to file: {}", file_path_.string());
  }
  archive_ = std::make_unique<boost::archive::binary_oarchive>(file_);

  SUBSCRIBE_TO_EVENT(event_handler, &MarketStreamSaver::OnMarketStreamEvent);
}

MarketStreamSaver::~MarketStreamSaver() {
  spdlog::warn("closing the stream file.");
  file_.close();
}

std::string MarketStreamSaver::filename() { return "market_stream.bin"; }

void MarketStreamSaver::OnMarketStreamEvent(MQ::Event event, const void* data) {
  switch (event) {
    case MQ::Event::kNewTradeEvent:
      spdlog::debug("write new trade");
      *archive_ << types::MarketDataType::TRADE
                << *static_cast<const market_stream::types::Trade*>(data);
      break;

    case MQ::Event::kOrderBookUpdateEvent:
      spdlog::debug("write order book update");
      *archive_ << types::MarketDataType::ORDER_BOOK
                << *static_cast<const market_stream::types::OrderBook*>(data);
      break;
    default:
      spdlog::error("MarketStreamPrinter unkown event {}", static_cast<int>(event));
      break;
  }
}

}  // namespace market_stream