#include <gmock/gmock.h>

#include "analyzer/benchmark_orchestrator.h"
#include "analyzer/order_book_snapshot_provider.h"
#include "analyzer/scoped_unit_state.h"
#include "events/event_hub.h"
#include "utils/tests/helpers/fake_market_stream_forwarder.h"
#include "utils/tests/helpers/unit_state_mock.h"

namespace analyzer {

TEST(OrderBookSnapshotProvider,
     GivenOrderBookStream_WhenHandleDiffStream_ThenOrderBookManagedCorrectly) {
  using MQMarketStream = events::message_queues::MarketStream;
  using MQOrderBookStream = events::message_queues::OrderBookStream;

  // Given
  auto unit_state = std::make_shared<
      MockUnitState<ObservableUnits::UnitId::kOrderBookSnapshotProvider>>();
  events::EventHub<MQMarketStream> market_stream_event_hub;
  events::EventHub<MQOrderBookStream> order_book_event_hub;

  std::shared_ptr<FakeMarketStreamForwarder> fake_forwarder =
      std::make_shared<FakeMarketStreamForwarder>(market_stream_event_hub.dispatcher());
  fake_forwarder->Initialize();
  analyzer::OrderBookSnapshotProvider ob_provider(market_stream_event_hub.CreateHandler(),
                                                  order_book_event_hub.dispatcher(),
                                                  unit_state);
  testing::Sequence unit_state_seq;
  for (int i = 0; i < fake_forwarder->fake_stream_.size(); i++) {
    if (market_stream::types::MarketDataType::ORDER_BOOK ==
        fake_forwarder->fake_stream_[i].first) {
      EXPECT_CALL(*unit_state, SetBusy()).InSequence(unit_state_seq);
      EXPECT_CALL(*unit_state, SetReady()).InSequence(unit_state_seq);
    }
  }

  market_stream::types::OrderBook dispatcher_snapshot;
  order_book_event_hub.CreateHandler().lock()->Subscribe(
      [&dispatcher_snapshot](MQOrderBookStream::Event event, const void* data) {
        dispatcher_snapshot = *static_cast<const market_stream::types::OrderBook*>(data);
      });

  // When
  fake_forwarder->Start();
  market_stream_event_hub.Shutdown();
  order_book_event_hub.Shutdown();
  auto order_book_snapshot = ob_provider.GetSnapshot();

  // Then
  fake_forwarder->VerifyOrderBookHandle(order_book_snapshot);
  fake_forwarder->VerifyOrderBookHandle(dispatcher_snapshot);
}

TEST(OrderBookSnapshotProvider,
     GivenOrderBookStream_WhenGetLatestUpdatedOrderBook_ThenGotLatestOrderBook) {
  using MQMarketStream = events::message_queues::MarketStream;
  using MQOrderBookStream = events::message_queues::OrderBookStream;

  // Given
  auto unit_state = std::make_shared<
      MockUnitState<ObservableUnits::UnitId::kOrderBookSnapshotProvider>>();
  events::EventHub<MQMarketStream> market_stream_event_hub;
  events::EventHub<MQOrderBookStream> order_book_event_hub;

  std::shared_ptr<FakeMarketStreamForwarder> fake_forwarder =
      std::make_shared<FakeMarketStreamForwarder>(market_stream_event_hub.dispatcher());
  fake_forwarder->Initialize();
  analyzer::OrderBookSnapshotProvider ob_provider(market_stream_event_hub.CreateHandler(),
                                                  order_book_event_hub.dispatcher(),
                                                  unit_state);
  testing::Sequence unit_state_seq;
  for (int i = 0; i < fake_forwarder->fake_stream_.size(); i++) {
    if (market_stream::types::MarketDataType::ORDER_BOOK ==
        fake_forwarder->fake_stream_[i].first) {
      EXPECT_CALL(*unit_state, SetBusy()).InSequence(unit_state_seq);
      EXPECT_CALL(*unit_state, SetReady()).InSequence(unit_state_seq);
    }
  }

  // When
  fake_forwarder->Start();
  auto order_book_snapshot = ob_provider.GetSnapshot(155466);

  // Then
  fake_forwarder->VerifyOrderBookHandle(order_book_snapshot);
}

}  // namespace analyzer