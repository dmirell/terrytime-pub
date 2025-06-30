#ifndef INCLUDE_MARKET_STREAM_I_BINAPI_CLIENT_H_
#define INCLUDE_MARKET_STREAM_I_BINAPI_CLIENT_H_

#include <binapi/types.hpp>
#include <functional>
#include <string>

#include "types/types.h"

namespace market_stream {

class IBinAPIClient {
 public:
  using TradeRawCallback = std::function<void(binapi::ws::trade_t&& trade_cb)>;
  using DepthRawCallback = std::function<void(binapi::rest::depths_t&& depths)>;
  using DepthUpdateRawCallback =
      std::function<void(binapi::ws::diff_depths_t&& diff_depths)>;

  IBinAPIClient(const std::string& symbol) : symbol_(symbol) {}
  virtual ~IBinAPIClient() = default;

  virtual void SubscribeToDepthStream(
      const DepthUpdateRawCallback& depth_book_update_cb) = 0;
  virtual void SubscribeToTradeStream(const TradeRawCallback& trade_cb) = 0;
  virtual void GetDepthAsync(const DepthRawCallback& depth_callback) = 0;
  virtual void Run() = 0;

 protected:
  std::string symbol_;
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_I_BINAPI_CLIENT_H_