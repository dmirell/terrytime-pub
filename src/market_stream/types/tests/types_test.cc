#include "market_stream/types/types.h"

#include <gtest/gtest.h>

#include <binapi/types.hpp>

TEST(MarketStreamTypes, GivenBinAPIDiffDepth_WhenMoveToOrderBook_ThenMoved) {
  // Given
  binapi::ws::diff_depths_t diff_depth;
  diff_depth.E = 1234567891011ll;
  diff_depth.s = "BTCUSDT";
  diff_depth.u = 123324324324ll;
  diff_depth.U = 574857834578ll;
  diff_depth.a = {{1.05, 2.04}, {2.4, 7.08}};
  diff_depth.b = {{3.02, 5.01}, {5.2, 6.22}, {51.2, 633.22}};
  auto original_diff_depth = diff_depth;

  // When
  market_stream::types::OrderBook order_book(std::move(diff_depth));

  // Then
  EXPECT_EQ(order_book.timestamp, original_diff_depth.E);
  ASSERT_EQ(order_book.bids.size(), original_diff_depth.b.size());
  ASSERT_EQ(order_book.asks.size(), original_diff_depth.a.size());
  for (int i = 0; i < order_book.bids.size(); i++) {
    EXPECT_EQ(order_book.bids[i].price, original_diff_depth.b[i].price);
    EXPECT_EQ(order_book.bids[i].quantity, original_diff_depth.b[i].amount);
  }
  for (int i = 0; i < order_book.asks.size(); i++) {
    EXPECT_EQ(order_book.asks[i].price, original_diff_depth.a[i].price);
    EXPECT_EQ(order_book.asks[i].quantity, original_diff_depth.a[i].amount);
  }
}

TEST(MarketStreamTypes, GivenBinAPIDepth_WhenMoveToOrderBook_ThenMoved) {
  // Given
  binapi::rest::depths_t depth;
  depth.asks = {{3.02, 5.01}, {5.2, 6.22}, {51.2, 633.22}};
  depth.bids = {{1.05, 2.04}, {2.4, 7.08}};
  auto original_depth = depth;

  // When
  market_stream::types::OrderBook order_book(std::move(depth));

  // Then
  ASSERT_EQ(order_book.bids.size(), original_depth.bids.size());
  ASSERT_EQ(order_book.asks.size(), original_depth.asks.size());
  for (int i = 0; i < order_book.bids.size(); i++) {
    EXPECT_EQ(order_book.bids[i].price, original_depth.bids[i].price);
    EXPECT_EQ(order_book.bids[i].quantity, original_depth.bids[i].amount);
  }
  for (int i = 0; i < order_book.asks.size(); i++) {
    EXPECT_EQ(order_book.asks[i].price, original_depth.asks[i].price);
    EXPECT_EQ(order_book.asks[i].quantity, original_depth.asks[i].amount);
  }
}

TEST(MarketStreamTypes, GivenBinAPITrade_WhenMoveToTrade_ThenMoved) {
  // Given
  binapi::ws::trade_t binapi_trade;
  binapi_trade.p = 1023.23;
  binapi_trade.q = 333.603;
  binapi_trade.m = true;
  binapi_trade.T = 1234567810101ll;
  binapi_trade.E = 1234567811111ll;
  auto original_binapi_trade = binapi_trade;

  // When
  market_stream::types::Trade trade(std::move(binapi_trade));

  // Then
  EXPECT_EQ(trade.price, original_binapi_trade.p);
  EXPECT_EQ(trade.quantity, original_binapi_trade.q);
  EXPECT_EQ(trade.is_buyer_maker, original_binapi_trade.m);
  EXPECT_EQ(trade.trade_timestamp, original_binapi_trade.T);
  EXPECT_EQ(trade.event_timestamp, original_binapi_trade.E);
}
