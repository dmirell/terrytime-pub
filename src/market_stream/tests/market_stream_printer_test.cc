

#include "market_stream/market_stream_printer.h"

#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "events/event_hub.h"
#include "utils/tests/helpers/fake_market_stream_forwarder.h"

TEST(MarketStreamPrinter, GiveMarketEvents_WhenRecieveEvents_ThenPrint) {
  using MQ = events::message_queues::MarketStream;
  // Given
  events::EventHub<MQ> event_hub;

  std::stringstream ss_sent_events, ss_recieved_events;
  std::shared_ptr<FakeMarketStreamForwarder> fake_forwarder =
      std::make_shared<FakeMarketStreamForwarder>(event_hub.dispatcher());
  fake_forwarder->Initialize();
  fake_forwarder->PrintStream(ss_sent_events);
  market_stream::MarketStreamPrinter printer(event_hub.CreateHandler(),
                                             ss_recieved_events);

  // When
  fake_forwarder->Start();
  event_hub.Shutdown();

  // Then
  EXPECT_EQ(ss_sent_events.str(), ss_recieved_events.str());
}