#include "market_stream/saved_market_stream_forwarder.h"

#include <spdlog/spdlog.h>

#include "analyzer/scoped_unit_state.h"
#include "market_stream/market_stream_saver.h"
#include "market_stream/types/types.h"

namespace market_stream {

SavedMarketStreamForwarder::SavedMarketStreamForwarder(
    const std::string& saved_file_path,
    const std::weak_ptr<EventHubDispatcher>& event_dispatcher,
    const std::shared_ptr<analyzer::SavedStreamForwarderUnitState>& unit_state)
    : file_path_(boost::filesystem::path(saved_file_path) /
                 MarketStreamSaver::filename()),
      event_dispatcher_(event_dispatcher),
      unit_state_(unit_state) {}

SavedMarketStreamForwarder::~SavedMarketStreamForwarder() {
  spdlog::warn("closing the market_stream file.");
  file_.close();
}

void SavedMarketStreamForwarder::Initialize() {
  spdlog::info("SavedMarketStreamForwarder initializing");
  spdlog::info("opening market_stream file");

  file_.open(file_path_.string(), std::ios::binary | std::ios::out);
  if (!file_.is_open()) {
    spdlog::error("Cannot open the file: {}", file_path_.string());
    exit(EXIT_FAILURE);
  } else {
    spdlog::info("Loading stream from file: {}", file_path_.string());
  }
  archive_ = std::make_unique<boost::archive::binary_iarchive>(file_);

  spdlog::info("MarketStreamForwarder initialized");
}

bool SavedMarketStreamForwarder::ReadNext(utils::Timestamp* next_data_ts) {
  using ScopedUnitState =
      analyzer::ScopedUnitState<analyzer::ObservableUnits::UnitId::kStreamForwarder>;
  std::unique_ptr<ScopedUnitState> scoped_state;
  if (nullptr != unit_state_) {
    scoped_state = std::make_unique<ScopedUnitState>(unit_state_);
  }
  types::MarketDataType data_type;
  if (!ReadNext(data_type)) {
    return false;
  }

  if (types::MarketDataType::ORDER_BOOK == data_type) {
    types::OrderBook order_book;
    if (!ReadNext(order_book)) {
      spdlog::error("Not able to read order_book as next data");
      return false;
    }
    if (nullptr != next_data_ts) {
      *next_data_ts = order_book.received_timestamp;
    }
    forward_next_func_ = [this, order_book]() {
      auto event_dispatcher_locked = event_dispatcher_.lock();
      if (nullptr == event_dispatcher_locked) {
        spdlog::error("Event dispatcher not available");
        exit(-1);
      }
      event_dispatcher_locked->DispatchEvent<MQ::Event::kOrderBookUpdateEvent>(
          order_book);
    };
  } else if (types::MarketDataType::TRADE == data_type) {
    types::Trade trade;
    if (!ReadNext(trade)) {
      spdlog::error("Not able to read trade as next data");
      return false;
    }
    if (nullptr != next_data_ts) {
      *next_data_ts = trade.received_timestamp;
    }
    forward_next_func_ = [this, trade]() {
      auto event_dispatcher_locked = event_dispatcher_.lock();
      if (nullptr == event_dispatcher_locked) {
        spdlog::error("Event dispatcher not available");
        exit(-1);
      }
      event_dispatcher_locked->DispatchEvent<MQ::Event::kNewTradeEvent>(trade);
    };
  } else {
    spdlog::error("unknown data_type");
    exit(-1);
  }
  return true;
}

void SavedMarketStreamForwarder::ForwardNext() {
  if (nullptr != forward_next_func_) {
    forward_next_func_();
    forward_next_func_ = nullptr;
  } else {
    spdlog::warn("nothing to forward. hint: read first");
  }
}

}  // namespace market_stream