#ifndef INCLUDE_MARKET_STREAM_TRADE_STREAM_FORWARDER_H_
#define INCLUDE_MARKET_STREAM_TRADE_STREAM_FORWARDER_H_

#include <binapi/types.hpp>
#include <functional>
#include <future>
#include <memory>

#include "i_stream_forwarder.h"
#include "types/types.h"

namespace market_stream {

class IBinAPIClient;
class TradeStreamForwarder : public IStreamForwarder {
 public:
  using TradeCallback = std::function<void(types::Trade &&)>;

  TradeStreamForwarder() = delete;
  TradeStreamForwarder(const TradeStreamForwarder &) = delete;
  TradeStreamForwarder(TradeStreamForwarder &&) = delete;
  TradeStreamForwarder &operator=(const TradeStreamForwarder &) = delete;
  TradeStreamForwarder &operator=(TradeStreamForwarder &&) = delete;

  TradeStreamForwarder(const std::shared_ptr<IBinAPIClient> &binapi_client,
                       const TradeCallback &trade_cb);
  ~TradeStreamForwarder() = default;

  // IStreamForwarder
  virtual std::future<void> StartAsync() override;

 protected:
  TradeCallback trade_cb_;

 private:
  void OnBinAPINewTrade(binapi::ws::trade_t &&trade);

  std::atomic<bool> stream_started_{false};
  std::shared_ptr<IBinAPIClient> binapi_client_;
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_TRADE_STREAM_FORWARDER_H_