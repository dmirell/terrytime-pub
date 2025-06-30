#include <gmock/gmock.h>

#include "analyzer/i_trading_strategy.h"
#include "analyzer/market_analyzer.h"
#include "events/event_hub.h"
#include "market_stream/types/types.h"
#include "utils/tests/helpers/unit_state_mock.h"

namespace analyzer {

namespace {
class MockTradingStrategy : public analyzer::ITradingStrategy {
 public:
  MockTradingStrategy(const std::weak_ptr<MQAnalyzerStreamDispatcher>& dispatcher)
      : ITradingStrategy(dispatcher) {}
  MOCK_METHOD(void, NewTrade, (const market_stream::types::Trade& trade), (override));
  MOCK_METHOD(void, OrderBookUpdate, (const market_stream::types::OrderBook& update),
              (override));
};
}  // namespace

TEST(MarketAnalyzer, GivenMarketAnalyzer_WhenOrderBookUpdateSent_ThenForwardToStrategy) {
  // Given
  auto unit_state = std::make_shared<MockUnitState<ObservableUnits::UnitId::kAnalyzer>>();
  events::EventHub<events::message_queues::MarketStream> ms_event_hub;
  events::EventHub<events::message_queues::AnalyzerStream> as_event_hub;

  auto strategy = std::make_shared<MockTradingStrategy>(as_event_hub.dispatcher());
  MarketAnalyzer analyzer(ms_event_hub.CreateHandler(), strategy, unit_state);

  // Then
  market_stream::types::OrderBook order_book;
  order_book.received_timestamp = 123;
  EXPECT_CALL(*strategy, OrderBookUpdate(order_book));

  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state, SetReady()).InSequence(unit_state_seq);

  // When
  ms_event_hub.dispatcher()
      .lock()
      ->DispatchEvent<events::message_queues::MarketStream::Event::kOrderBookUpdateEvent>(
          order_book);

  ms_event_hub.Shutdown();
}

TEST(MarketAnalyzer, GivenMarketAnalyzer_WhenNewTradeSent_ThenForwardToStrategy) {
  // Given
  auto unit_state = std::make_shared<MockUnitState<ObservableUnits::UnitId::kAnalyzer>>();
  events::EventHub<events::message_queues::MarketStream> ms_event_hub;
  events::EventHub<events::message_queues::AnalyzerStream> as_event_hub;

  auto strategy = std::make_shared<MockTradingStrategy>(as_event_hub.dispatcher());
  MarketAnalyzer analyzer(ms_event_hub.CreateHandler(), strategy, unit_state);

  // Then
  market_stream::types::Trade trade;
  trade.received_timestamp = 123;
  EXPECT_CALL(*strategy, NewTrade(trade));

  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state, SetReady()).InSequence(unit_state_seq);

  // When
  ms_event_hub.dispatcher()
      .lock()
      ->DispatchEvent<events::message_queues::MarketStream::Event::kNewTradeEvent>(trade);

  ms_event_hub.Shutdown();
}
}  // namespace analyzer