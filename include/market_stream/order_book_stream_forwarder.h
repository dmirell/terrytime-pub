#ifndef INCLUDE_MARKET_STREAM_ORDER_BOOK_STREAM_FORWARDER_H_
#define INCLUDE_MARKET_STREAM_ORDER_BOOK_STREAM_FORWARDER_H_

#include <binapi/types.hpp>
#include <cctype>
#include <functional>
#include <memory>
#include <vector>

#include "i_stream_forwarder.h"
#include "types/types.h"

namespace market_stream {

class IBinAPIClient;

class OrderBookStreamForwarder : public IStreamForwarder {
 public:
  OrderBookStreamForwarder() = delete;
  OrderBookStreamForwarder(const OrderBookStreamForwarder &) = delete;
  OrderBookStreamForwarder(OrderBookStreamForwarder &&) = delete;
  OrderBookStreamForwarder &operator=(const OrderBookStreamForwarder &) = delete;
  OrderBookStreamForwarder &operator=(OrderBookStreamForwarder &&) = delete;

  using OrderBookUpdateCallback = std::function<void(types::OrderBook &&)>;
  OrderBookStreamForwarder(const std::shared_ptr<IBinAPIClient> &binapi_client,
                           const OrderBookUpdateCallback &order_book_update_cb);
  ~OrderBookStreamForwarder() = default;

  // IStreamForwarder
  virtual std::future<void> StartAsync() override;

 protected:
  OrderBookUpdateCallback order_book_update_cb_;

 private:
  void OnBinAPIDepthUpdate(binapi::ws::diff_depths_t &&diff_depths);
  void OnBinAPIDepthRecieve(binapi::rest::depths_t &&depths);
  void ProcessBufferedUpdates();

  std::vector<binapi::ws::diff_depths_t>::iterator SearchFirstRealUpdate(
      uint64_t last_update_id);

  enum class InitStage {
    WAIT_FOR_FIRST_BUFFER_UPDATE = 0,
    DEPTH_SNAPSHOT_REQUESTED,
    SEARCH_FOR_FIRST_REAL_UPDATE,
    STREAM_STARTED
  };

  InitStage init_stage_;
  uint64_t depth_last_update_id_{0};
  uint64_t previous_last_update_id_{0};
  std::shared_ptr<IBinAPIClient> binapi_client_;
  std::vector<binapi::ws::diff_depths_t> depth_updates_buffer_;
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_ORDER_BOOK_STREAM_FORWARDER_H_