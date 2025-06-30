#include <gmock/gmock.h>

#include <boost/filesystem.hpp>
#include <chrono>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <vector>

#include "events/event_hub.h"
#include "market_stream/market_stream_saver.h"
#include "market_stream/saved_market_stream_forwarder.h"
#include "market_stream/types/types.h"
#include "utils/tests/helpers/fake_market_stream_forwarder.h"
#include "utils/tests/helpers/unit_state_mock.h"

namespace {
namespace fs = boost::filesystem;

class StorageFixture : public ::testing::Test {
 public:
  fs::path temp_dir() const { return temp_dir_; }

 protected:
  fs::path temp_dir_;

  void SetUp() override {
    temp_dir_ = fs::temp_directory_path() / fs::unique_path();
    fs::create_directory(temp_dir_);
  }

  void TearDown() override {
    if (fs::exists(temp_dir_)) {
      fs::remove_all(temp_dir_);
    }
  }
};
}  // namespace

TEST_F(StorageFixture, GivenMarketStream_WhenStreamSaved_ThenCanBeLoaded) {
  using MQ = events::message_queues::MarketStream;
  // Given
  events::EventHub<MQ> event_hub1;

  std::shared_ptr<FakeMarketStreamForwarder> fake_forwarder =
      std::make_shared<FakeMarketStreamForwarder>(event_hub1.dispatcher());
  fake_forwarder->Initialize();

  // When
  {
    market_stream::MarketStreamSaver stream_saver(event_hub1.CreateHandler(),
                                                  temp_dir().string());
    fake_forwarder->Start();
    event_hub1.Shutdown();
  }

  // Then
  events::EventHub<MQ> event_hub2;
  events::EventHub<MQ> event_hub3;

  std::vector<std::pair<market_stream::types::MarketDataType,
                        FakeMarketStreamForwarder::CombinedData>>
      recieved_stream;

  std::shared_ptr<market_stream::SavedMarketStreamForwarder> saved_forwarder =
      std::make_shared<market_stream::SavedMarketStreamForwarder>(
          temp_dir().string(), event_hub2.dispatcher());
  saved_forwarder->Initialize();
  auto unit_state = std::make_shared<
      MockUnitState<analyzer::ObservableUnits::UnitId::kStreamForwarder>>();

  testing::Sequence unit_state_seq;
  for (int i = 0; i < fake_forwarder->fake_stream_.size() + 1; i++) {  // +1 for EOF check
    EXPECT_CALL(*unit_state, SetBusy()).InSequence(unit_state_seq);
    EXPECT_CALL(*unit_state, SetReady()).InSequence(unit_state_seq);
  }

  std::shared_ptr<market_stream::SavedMarketStreamForwarder> saved_forwarder2 =
      std::make_shared<market_stream::SavedMarketStreamForwarder>(
          temp_dir().string(), event_hub3.dispatcher(), unit_state);
  saved_forwarder2->Initialize();

  auto on_market_stream_cb = [&recieved_stream](MQ::Event event, const void* data) {
    FakeMarketStreamForwarder::CombinedData combined_data;
    switch (event) {
      case MQ::Event::kNewTradeEvent:
        combined_data.trade = *static_cast<const market_stream::types::Trade*>(data);
        recieved_stream.push_back(
            {market_stream::types::MarketDataType::TRADE, combined_data});
        break;

      case MQ::Event::kOrderBookUpdateEvent:
        combined_data.order_book =
            *static_cast<const market_stream::types::OrderBook*>(data);
        recieved_stream.push_back(
            {market_stream::types::MarketDataType::ORDER_BOOK, combined_data});
        break;
    }
  };
  event_hub2.CreateHandler().lock()->Subscribe(on_market_stream_cb);
  event_hub3.CreateHandler().lock()->Subscribe(on_market_stream_cb);

  while (saved_forwarder->ReadNext()) {
    saved_forwarder->ForwardNext();
  }

  event_hub2.Shutdown();

  fake_forwarder->VerifyRecievedStream(recieved_stream);
  recieved_stream.clear();

  while (saved_forwarder2->ReadNext()) {
    saved_forwarder2->ForwardNext();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  event_hub3.Shutdown();

  fake_forwarder->VerifyRecievedStream(recieved_stream);
}