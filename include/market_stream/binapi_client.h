#ifndef INCLUDE_MARKET_STREAM_BINAPI_CLIENT_H_
#define INCLUDE_MARKET_STREAM_BINAPI_CLIENT_H_

#include <binapi/api.hpp>
#include <binapi/websocket.hpp>
#include <boost/asio/io_context.hpp>
#include <memory>
#include <thread>

#include "i_binapi_client.h"
#include "types/types.h"

namespace market_stream {

class BinAPIClient : public IBinAPIClient {
 public:
  BinAPIClient() = delete;
  BinAPIClient(const BinAPIClient&) = delete;
  BinAPIClient(BinAPIClient&&) = delete;
  BinAPIClient& operator=(const BinAPIClient&) = delete;
  BinAPIClient& operator=(BinAPIClient&&) = delete;

  BinAPIClient(const std::string& symbol);
  ~BinAPIClient();

  // IBinAPIClient
  void SubscribeToDepthStream(
      const DepthUpdateRawCallback& depth_book_update_cb) override;
  void SubscribeToTradeStream(const TradeRawCallback& trade_cb) override;
  void GetDepthAsync(const DepthRawCallback& depth_callback) override;

  void Run();

 private:
  std::atomic<bool> stop_monitor_thread_{false};
  bool run_ws_combined_stream_{false};
  std::thread monitor_thread_;
  std::unique_ptr<binapi::rest::api> api_;
  std::unique_ptr<binapi::ws::websockets> ws_;
  boost::asio::io_context io_context_;
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_BINAPI_CLIENT_H_