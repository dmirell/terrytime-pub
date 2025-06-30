#ifndef INCLUDE_MARKET_STREAM_TYPES_H_
#define INCLUDE_MARKET_STREAM_TYPES_H_

#include <fmt/format.h>

#include <binapi/types.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <cctype>
#include <vector>

namespace market_stream {
namespace types {

using DoubleType = binapi::double_type;

enum class MarketDataType { ORDER_BOOK = 0, TRADE };
template <class Archive>
void serialize(Archive& ar, MarketDataType& type, const unsigned int version) {
  ar & type;
}

struct OrderBook {
  struct Item {
    DoubleType price;
    DoubleType quantity;

    Item() = default;
    Item(const Item&) = default;
    Item& operator=(const Item&) = default;
    Item(Item&&) = default;
    Item& operator=(Item&&) = default;

    Item(DoubleType _price, DoubleType _quantity) : price(_price), quantity(_quantity) {}
    Item(binapi::rest::depths_t::depth_t&& depth) noexcept;
    Item(binapi::ws::diff_depths_t::depth_t&& depth) noexcept;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
      ar & price & quantity;
    }

    friend std::ostream& operator<<(std::ostream& os, const Item& o);
  };

  using Items = std::vector<Item>;

  uint64_t timestamp{0};           // ms
  uint64_t received_timestamp{0};  // ms
  Items bids;
  Items asks;

  OrderBook() = default;
  OrderBook(const OrderBook&) = default;
  OrderBook& operator=(const OrderBook&) = default;
  OrderBook(OrderBook&&) = default;
  OrderBook& operator=(OrderBook&&) = default;

  OrderBook(binapi::rest::depths_t&& depths) noexcept;
  OrderBook(binapi::ws::diff_depths_t&& diff_depths) noexcept;

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & timestamp & received_timestamp & bids & asks;
  }

  friend std::ostream& operator<<(std::ostream& os, const OrderBook& o);
  friend std::ostream& operator<<(std::ostream& os, const Items& o);
};

struct Trade {
  DoubleType price;
  DoubleType quantity;
  bool is_buyer_maker;
  uint64_t trade_timestamp;        // ms
  uint64_t event_timestamp;        // ms
  uint64_t received_timestamp{0};  // ms

  Trade() = default;
  Trade(binapi::ws::trade_t&& trade) noexcept;

  Trade(const Trade&) = default;
  Trade& operator=(const Trade&) = default;

  Trade(Trade&&) = delete;
  Trade& operator=(Trade&&) = delete;

  void print() const;

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & price & quantity & is_buyer_maker & trade_timestamp & event_timestamp &
        received_timestamp;
  }

  friend std::ostream& operator<<(std::ostream& os, const Trade& o);
};

bool operator==(const Trade& a, const Trade& b) noexcept;
bool operator==(const OrderBook& a, const OrderBook& b) noexcept;
bool operator==(const OrderBook::Item& a, const OrderBook::Item& b) noexcept;

}  // namespace types
}  // namespace market_stream

namespace fmt {
template <>
struct formatter<market_stream::types::DoubleType> {
  // Parses the format specifier (optional, can be ignored)
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  // Formats the Decimal number as a string
  template <typename FormatContext>
  auto format(const market_stream::types::DoubleType& value, FormatContext& ctx)
      -> decltype(ctx.out()) {
    return format_to(ctx.out(), "{}", value.str(8, std::ios::fixed));
  }
};
}  // namespace fmt

#endif  // INCLUDE_MARKET_STREAM_TYPES_H_