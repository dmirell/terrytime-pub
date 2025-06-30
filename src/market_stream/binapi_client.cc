#include "market_stream/binapi_client.h"

#include <spdlog/spdlog.h>

#include <cassert>
#include <chrono>
#include <ctime>
#include <sstream>
#include <string>

namespace {
const std::string gHost = "api.binance.com";
const std::string gWSHost = "stream.binance.com";
const int gDepthRequestLevels = 5000;
}  // namespace

namespace market_stream {
std::string getThreadIdString() {
  // On C++11 and later, use std::this_thread::get_id() to get the thread ID
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  return oss.str();
}

BinAPIClient::BinAPIClient(const std::string& symbol) : IBinAPIClient(symbol) {
  ws_ = std::make_unique<binapi::ws::websockets>(io_context_, gWSHost, "9443");
  api_ = std::make_unique<binapi::rest::api>(io_context_, gHost, "443", "", "", 10000);
  spdlog::info("BinAPIClient initialized.");
}

void BinAPIClient::SubscribeToDepthStream(const DepthUpdateRawCallback& depth_update_cb) {
  spdlog::debug("connect to diff_depth Binance stream...");
  assert(depth_update_cb != nullptr);
  run_ws_combined_stream_ = true;
  ws_->add_diff_depth_to_combined_stream(
      binapi::e_freq::_100ms, [this, depth_update_cb](auto diff_depths) {
        /*auto received_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::system_clock::now();

        // Convert the time point to milliseconds since the epoch
        auto milliseconds_since_epoch =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                current_time.time_since_epoch())
                .count();
        spdlog::info("Depth received tid={} timestamp: {} ms",
                     getThreadIdString(), milliseconds_since_epoch);*/

        if (spdlog::get_level() == spdlog::level::trace) {
          std::stringstream ss;
          ss << diff_depths;
          spdlog::trace("Diff depth: {}", ss.str());
        }

        // auto processing_start_time =
        // std::chrono::high_resolution_clock::now();

        depth_update_cb(std::move(diff_depths));

        /*
        // Время завершения обработки
        auto processing_end_time = std::chrono::high_resolution_clock::now();
        auto duration_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                processing_end_time - processing_start_time);
        spdlog::debug("Depth update processing time: {} ms",
                      duration_ms.count());

        auto total_duration_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                processing_end_time - received_time);
        spdlog::debug("Total time from received to processed: {} ms",
                      total_duration_ms.count());*/

        return true;
      });
}

void BinAPIClient::SubscribeToTradeStream(const TradeRawCallback& trade_cb) {
  spdlog::debug("connect to trade Binance stream...");
  assert(trade_cb != nullptr);
  run_ws_combined_stream_ = true;
  ws_->add_trade_to_combined_stream([trade_cb](auto trade) {
    /*auto received_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::system_clock::now();

    // Convert the time point to milliseconds since the epoch
    auto milliseconds_since_epoch =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time.time_since_epoch())
            .count();
    spdlog::info("Trade received tid={} timestamp: {} ms", getThreadIdString(),
                 milliseconds_since_epoch);*/

    if (spdlog::get_level() == spdlog::level::trace) {
      std::stringstream ss;
      ss << trade;
      spdlog::trace("Trade: {}", ss.str());
    }

    // auto processing_start_time = std::chrono::high_resolution_clock::now();

    trade_cb(std::move(trade));

    /*
    // Время завершения обработки
    auto processing_end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        processing_end_time - processing_start_time);
    spdlog::debug("Trade update processing time: {} ms", duration_ms.count());

    auto total_duration_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            processing_end_time - received_time);
    spdlog::debug("Total time from received to processed: {} ms",
                  total_duration_ms.count());*/

    return true;
  });
}

BinAPIClient::~BinAPIClient() {
  stop_monitor_thread_ = true;
  if (run_ws_combined_stream_) {
    spdlog::warn("BinAPIClient async_unsubscribe_all.");
    ws_->async_unsubscribe_all();
  }
  if (monitor_thread_.joinable()) {
    monitor_thread_.join();
  }
}

void BinAPIClient::GetDepthAsync(const DepthRawCallback& depth_callback) {
  assert(depth_callback != nullptr);
  api_->depths(symbol_, gDepthRequestLevels,
               [depth_callback](const char* fl, int ec, std::string errmsg, auto res) {
                 if (ec) {
                   spdlog::error("get depth error: fl={}, ec={}, emsg={}", fl, ec,
                                 errmsg);
                   exit(-1);
                 }
                 if (spdlog::get_level() == spdlog::level::trace) {
                   std::stringstream ss;
                   ss << res;
                   spdlog::trace("Depth diagram recieved: {}", ss.str());
                 }

                 depth_callback(std::move(res));
                 return true;
               });
  if (!run_ws_combined_stream_) {
    monitor_thread_ = std::thread([this]() {
      spdlog::info("io_context run started");
      io_context_.reset();
      io_context_.run();
      spdlog::warn("io_context run finished");
    });
    monitor_thread_.detach();
  }
  spdlog::debug("BinAPIClient GET depths request sent.");
}
void BinAPIClient::Run() {
  if (run_ws_combined_stream_) {
    ws_->combined_stream_run(symbol_.c_str(), [](const char* fl, int ec, std::string emsg,
                                                 auto combined_stream) {
      spdlog::error("combined_stream_run error: fl={}, ec={}, emsg={}", fl, ec, emsg);
      exit(-1);
    });
    monitor_thread_ = std::thread([this]() {
      spdlog::info("io_context run started");
      io_context_.run();
      spdlog::warn("io_context run finished");
    });
  }
}

}  // namespace market_stream