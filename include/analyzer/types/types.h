#ifndef INCLUDE_ANALYZER_TYPES_TYPES_H_
#define INCLUDE_ANALYZER_TYPES_TYPES_H_

#include <cctype>
#include <string>
#include <vector>

#include "market_stream/types/types.h"
#include "utils/time/types.h"

namespace analyzer {
namespace types {

struct OrderPlanInfo {
  market_stream::types::DoubleType max_buy_price{0.0};
  market_stream::types::DoubleType quantity{0.0};
  market_stream::types::DoubleType min_sell_price{0.0};
  utils::Timestamp expiration_ts;
  utils::Timestamp expiration_buy_ts;
  utils::Timestamp expiration_sell_ts;
};

struct OrderInfo {
  enum class Side { BUY = 0, SELL };
  enum class Type { MARKET = 0, LIMIT };

  std::string symbol;
  Side side{Side::BUY};
  Type type{Type::MARKET};
  market_stream::types::DoubleType quantity{0.0};
  market_stream::types::DoubleType price{0.0};
};

bool operator==(const OrderInfo&, const OrderInfo&) noexcept;

struct OrderResult {
  enum class Status { kOk = 0, kCanceled, kFailed };

  Status status{Status::kOk};
  uint64_t order_id{0};
  market_stream::types::DoubleType price{0.0};
  market_stream::types::DoubleType quantity{0.0};
};

template <template <typename> class SyncSide>
struct OrderResponse {
  SyncSide<uint64_t> order_id;
  SyncSide<OrderResult> order_result;
};

struct OrderPlanReport {
  enum class Status { kOk = 0, kFailedToProfitSell, kFailedToBuy, kPlanExpired, COUNT };

  market_stream::types::DoubleType buy_price{0.0};
  market_stream::types::DoubleType sold_price{0.0};
  Status status{Status::kOk};
  utils::Timestamp start_ts{0};  // ms
  utils::Timestamp duration{0};  // ms

  static std::string StatusToString(Status s);

  bool operator==(const OrderPlanReport& other) const;
};

}  // namespace types
}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_TYPES_TYPES_H_