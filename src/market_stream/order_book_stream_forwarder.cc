#include "market_stream/order_book_stream_forwarder.h"

#include <spdlog/spdlog.h>

#include <cassert>

#include "market_stream/binapi_client.h"

namespace market_stream {

OrderBookStreamForwarder::OrderBookStreamForwarder(
    const std::shared_ptr<IBinAPIClient>& binapi_client,
    const OrderBookUpdateCallback& order_book_update_cb)
    : binapi_client_(binapi_client),
      order_book_update_cb_(order_book_update_cb),
      init_stage_(InitStage::WAIT_FOR_FIRST_BUFFER_UPDATE) {
  binapi_client_->SubscribeToDepthStream(std::bind(
      &OrderBookStreamForwarder::OnBinAPIDepthUpdate, this, std::placeholders::_1));
  spdlog::info("OrderBookStreamForwarder initialized");
}

std::future<void> OrderBookStreamForwarder::StartAsync() {
  return stream_started_promise_.get_future();
}

void OrderBookStreamForwarder::OnBinAPIDepthUpdate(
    binapi::ws::diff_depths_t&& diff_depths) {
  spdlog::debug("OnBinAPIDepthUpdate");
  switch (init_stage_) {
    case InitStage::WAIT_FOR_FIRST_BUFFER_UPDATE:
      spdlog::debug("First update recieved. Send get depth request.");
      depth_updates_buffer_.emplace_back(std::move(diff_depths));
      binapi_client_->GetDepthAsync(std::bind(
          &OrderBookStreamForwarder::OnBinAPIDepthRecieve, this, std::placeholders::_1));
      init_stage_ = InitStage::DEPTH_SNAPSHOT_REQUESTED;
      break;
    case InitStage::DEPTH_SNAPSHOT_REQUESTED:
      spdlog::debug("Depth snapshot request sent. Update buffered.");
      depth_updates_buffer_.emplace_back(std::move(diff_depths));
      break;
    case InitStage::SEARCH_FOR_FIRST_REAL_UPDATE:
      spdlog::debug("Waiting for first real update. Update buffered.");
      depth_updates_buffer_.emplace_back(std::move(diff_depths));
      ProcessBufferedUpdates();
      break;
    case InitStage::STREAM_STARTED:
      assert((previous_last_update_id_ + 1 == diff_depths.U) &&
             "Wrong depth update id order");
      previous_last_update_id_ = diff_depths.u;
      order_book_update_cb_(std::move(diff_depths));
      break;
    default:
      spdlog::error("Not existing init state {} at order book forwarder",
                    static_cast<int>(init_stage_));
      exit(-1);
  }
}

void OrderBookStreamForwarder::OnBinAPIDepthRecieve(binapi::rest::depths_t&& depths) {
  spdlog::debug("OnBinAPIDepthRecieve recieved");
  depth_last_update_id_ = depths.lastUpdateId;
  order_book_update_cb_(std::move(depths));
  init_stage_ = InitStage::SEARCH_FOR_FIRST_REAL_UPDATE;
  ProcessBufferedUpdates();
}

std::vector<binapi::ws::diff_depths_t>::iterator
OrderBookStreamForwarder::SearchFirstRealUpdate(uint64_t last_update_id) {
  for (auto it = depth_updates_buffer_.begin(); it != depth_updates_buffer_.end(); ++it) {
    if (it->U <= last_update_id + 1 && last_update_id + 1 <= it->u) {
      return it;
    }
  }
  return depth_updates_buffer_.end();
}

void OrderBookStreamForwarder::ProcessBufferedUpdates() {
  auto first_update_to_proceed_it = SearchFirstRealUpdate(depth_last_update_id_);
  if (first_update_to_proceed_it != depth_updates_buffer_.end()) {
    spdlog::debug("First real update to start proceed found.");
    for (auto it = first_update_to_proceed_it; it != depth_updates_buffer_.end(); ++it) {
      spdlog::debug("Fire buffered update.");
      order_book_update_cb_(std::move(*it));
      previous_last_update_id_ = it->u;
    }
    depth_updates_buffer_.clear();
    spdlog::info("Order book stream started successfully.");
    init_stage_ = InitStage::STREAM_STARTED;
    stream_started_promise_.set_value();
  }
}
}  // namespace market_stream