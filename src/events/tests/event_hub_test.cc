#include "events/event_hub.h"

#include <gmock/gmock.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "market_stream/types/types.h"
#include "utils/tests/helpers/unit_state_mock.h"

using MQ = events::message_queues::MarketStream;
namespace {
void SendDummyOrdeBookEvent(std::shared_ptr<events::EventHub<MQ>::Dispatcher> dispather) {
  dispather->DispatchEvent<MQ::Event::kOrderBookUpdateEvent>(
      market_stream::types::OrderBook());
}

void SendDummyTradeEvent(std::shared_ptr<events::EventHub<MQ>::Dispatcher> dispather) {
  dispather->DispatchEvent<MQ::Event::kNewTradeEvent>(market_stream::types::Trade());
}

class EventHubFixture : public ::testing::Test {
 public:
  EventHubFixture()
      : unit_state_(std::make_shared<
                    MockUnitState<analyzer::ObservableUnits::UnitId::kTransport>>()) {}
  void SetUp() { events::SetTransportUnitState(unit_state_); }
  void TearDown() { events::SetTransportUnitState(nullptr); }

  std::shared_ptr<MockUnitState<analyzer::ObservableUnits::UnitId::kTransport>>
      unit_state_;
};

}  // namespace

TEST_F(EventHubFixture,
       GivenEventDispatcherSubscribers_WhenEventsSent_ThenRecievedInSameOrder) {
  using namespace events;

  EXPECT_EQ(g_transport_unit_state_impl->total_event_count(), 0);

  // Given
  EventHub<MQ> event_loop;
  const int kSubscribersCount = 3;
  std::vector<MQ::Event> recieved_events[kSubscribersCount];
  auto handler_locked = event_loop.CreateHandler().lock();
  for (int i = 0; i < kSubscribersCount; i++) {
    auto& events_stack = recieved_events[i];
    handler_locked->Subscribe([&events_stack](MQ::Event event, const void* data) {
      switch (event) {
        case MQ::Event::kOrderBookUpdateEvent:
          events_stack.push_back(MQ::Event::kOrderBookUpdateEvent);
          break;

        case MQ::Event::kNewTradeEvent:
          events_stack.push_back(MQ::Event::kNewTradeEvent);
          break;
      }
    });
  }

  // When
  auto dispatcher_locked = event_loop.dispatcher().lock();
  SendDummyOrdeBookEvent(dispatcher_locked);
  SendDummyOrdeBookEvent(dispatcher_locked);
  SendDummyTradeEvent(dispatcher_locked);
  SendDummyOrdeBookEvent(dispatcher_locked);
  SendDummyTradeEvent(dispatcher_locked);
  SendDummyOrdeBookEvent(dispatcher_locked);
  SendDummyTradeEvent(dispatcher_locked);
  SendDummyTradeEvent(dispatcher_locked);

  event_loop.Shutdown();

  EXPECT_EQ(g_transport_unit_state_impl->total_event_count(), 0);

  // Then
  for (int i = 0; i < kSubscribersCount; i++) {
    const auto& events_stack = recieved_events[i];
    ASSERT_EQ(events_stack.size(), 8);
    EXPECT_EQ(events_stack[0], MQ::Event::kOrderBookUpdateEvent);
    EXPECT_EQ(events_stack[1], MQ::Event::kOrderBookUpdateEvent);
    EXPECT_EQ(events_stack[2], MQ::Event::kNewTradeEvent);
    EXPECT_EQ(events_stack[3], MQ::Event::kOrderBookUpdateEvent);
    EXPECT_EQ(events_stack[4], MQ::Event::kNewTradeEvent);
    EXPECT_EQ(events_stack[5], MQ::Event::kOrderBookUpdateEvent);
    EXPECT_EQ(events_stack[6], MQ::Event::kNewTradeEvent);
    EXPECT_EQ(events_stack[7], MQ::Event::kNewTradeEvent);
  }
}

TEST_F(EventHubFixture, GivenSluggishHandler_WhenEventsSent_ThenHandlerFinishedJob) {
  using namespace events;
  // Given
  EXPECT_EQ(g_transport_unit_state_impl->total_event_count(), 0);
  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);

  EventHub<MQ> event_hub;
  std::atomic<bool> handler_finished_job = false;
  event_hub.CreateHandler().lock()->Subscribe([&](MQ::Event event, const void* data) {
    std::this_thread::sleep_for(std::chrono::seconds(4));
    handler_finished_job = true;
  });

  // When
  auto dispatcher_locked = event_hub.dispatcher().lock();
  SendDummyOrdeBookEvent(dispatcher_locked);
  SendDummyOrdeBookEvent(dispatcher_locked);
  SendDummyOrdeBookEvent(dispatcher_locked);

  std::this_thread::sleep_for(std::chrono::seconds(2));
  EXPECT_EQ(g_transport_unit_state_impl->total_event_count(), 3);
  event_hub.Shutdown();
  EXPECT_EQ(g_transport_unit_state_impl->total_event_count(), 0);

  // Then
  EXPECT_TRUE(handler_finished_job);
}