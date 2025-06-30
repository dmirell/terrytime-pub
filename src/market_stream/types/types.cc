#include "market_stream/types/types.h"

#include <iomanip>
#include <iostream>

namespace market_stream {

namespace types {

OrderBook::Item::Item(binapi::rest::depths_t::depth_t&& depth) noexcept
    : price(depth.price), quantity(depth.amount) {}
OrderBook::Item::Item(binapi::ws::diff_depths_t::depth_t&& depth) noexcept
    : price(depth.price), quantity(depth.amount) {}

OrderBook::OrderBook(binapi::rest::depths_t&& depths) noexcept
    : timestamp(0),
      bids(std::make_move_iterator(depths.bids.begin()),
           std::make_move_iterator(depths.bids.end())),
      asks(std::make_move_iterator(depths.asks.begin()),
           std::make_move_iterator(depths.asks.end())) {}

OrderBook::OrderBook(binapi::ws::diff_depths_t&& diff_depths) noexcept
    : timestamp(diff_depths.E),
      bids(std::make_move_iterator(diff_depths.b.begin()),
           std::make_move_iterator(diff_depths.b.end())),
      asks(std::make_move_iterator(diff_depths.a.begin()),
           std::make_move_iterator(diff_depths.a.end())) {}

std::ostream& operator<<(std::ostream& os, const OrderBook::Item& o) {
  os << std::fixed << std::setprecision(6);
  os << "[" << o.price << ", " << o.quantity << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const OrderBook& o) {
  os << std::fixed << std::setprecision(6);
  os << "OrderBook: [timestamp: " << o.timestamp
     << " ms, r_timestamp: " << o.received_timestamp << " ms, ";
  os << "bids: " << o.bids;
  os << ", asks: " << o.asks;
  os << "]\n";
  return os;
}

std::ostream& operator<<(std::ostream& os, const OrderBook::Items& o) {
  os << "[";
  for (int i = 0; i < o.size(); i++) {
    os << o[i];
    if (i + 1 != o.size()) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

Trade::Trade(binapi::ws::trade_t&& trade) noexcept
    : price(trade.p),
      quantity(trade.q),
      is_buyer_maker(trade.m),
      event_timestamp(static_cast<uint64_t>(trade.E)),  // ms
      trade_timestamp(static_cast<uint64_t>(trade.T))   // ms
{}

std::ostream& operator<<(std::ostream& os, const Trade& o) {
  os << std::fixed << std::setprecision(6);
  os << "Trade: [e_timestamp: " << o.event_timestamp
     << " ms, t_timestamp: " << o.trade_timestamp
     << " ms, r_timestamp: " << o.received_timestamp << " ms, price: " << o.price
     << ", quantity: " << o.quantity << ", is_buyer_maker: " << o.is_buyer_maker << "]\n";
  return os;
}

bool operator==(const Trade& a, const Trade& b) noexcept {
  return a.event_timestamp == b.event_timestamp &&
         a.trade_timestamp == b.trade_timestamp &&
         a.received_timestamp == b.received_timestamp && a.price == b.price &&
         a.quantity == b.quantity && a.is_buyer_maker == b.is_buyer_maker;
}

bool operator==(const OrderBook& a, const OrderBook& b) noexcept {
  return a.timestamp == b.timestamp && a.received_timestamp == b.received_timestamp &&
         a.bids.size() == b.bids.size() && a.asks.size() == b.asks.size() &&
         std::equal(a.bids.begin(), a.bids.end(), b.bids.begin()) &&
         std::equal(a.asks.begin(), a.asks.end(), b.asks.begin());
}

bool operator==(const OrderBook::Item& a, const OrderBook::Item& b) noexcept {
  return a.price == b.price && a.quantity == b.quantity;
}

}  // namespace types

}  // namespace market_stream