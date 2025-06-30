#include "analyzer/types/types.h"

#include <iomanip>
#include <iostream>

#include "utils/helpers.h"

namespace analyzer {

namespace types {

bool operator==(const OrderInfo& l, const OrderInfo& r) noexcept {
  return utils::IsDoublesEqual(l.price, r.price) &&
         utils::IsDoublesEqual(l.quantity, r.quantity) && l.side == r.side &&
         l.symbol == r.symbol && l.type == r.type;
}

std::string OrderPlanReport::StatusToString(Status s) {
  std::string result;
  switch (s) {
    case Status::kOk:
      result = "ok";
      break;

    case Status::kFailedToBuy:
      result = "failed_to_buy";
      break;

    case Status::kFailedToProfitSell:
      result = "failed_to_profit_sell";
      break;

    case Status::kPlanExpired:
      result = "plan_expired";
      break;

    default:
      result = "unknown";
      break;
  }
  return result;
}

bool OrderPlanReport::operator==(const OrderPlanReport& other) const {
  return buy_price == other.buy_price && sold_price == other.sold_price &&
         // start_time == other.start_ts && duration == other.duration &&
         status == other.status;
}

}  // namespace types

}  // namespace analyzer