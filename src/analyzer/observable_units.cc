#include "analyzer/observable_units.h"

#include <spdlog/spdlog.h>

namespace analyzer {

ObservableUnits::ObservableUnits() : units_busy_(0) {}

ObservableUnits &ObservableUnits::Instance() {
  static ObservableUnits units;
  return units;
}

void ObservableUnits::SubscribeToAllUnitsReady(const UnitsReadyCallback &cb) {
  std::lock_guard<std::mutex> lock(mutex_);
  units_ready_cb_ = cb;
}

void ObservableUnits::Unsubscribe() {
  std::lock_guard<std::mutex> lock(mutex_);
  units_ready_cb_ = nullptr;
}

std::vector<ObservableUnits::UnitId> ObservableUnits::GetBusyUnits() const {
  std::lock_guard<std::mutex> lock(mutex_);
  std::vector<UnitId> result;
  for (int i = 0; i < static_cast<int>(UnitId::COUNT); i++) {
    if (units_busy_[i]) {
      result.push_back(static_cast<UnitId>(i));
    }
  }
  return result;
}

std::ostream &operator<<(std::ostream &os,
                         const std::vector<ObservableUnits::UnitId> &o) {
  os << "[";
  for (const auto &it : o) {
    os << static_cast<int>(it) << ", ";
  }
  os << "]";
  return os;
}

void ObservableUnits::SetReady(UnitId unit) {
  std::lock_guard<std::mutex> lock(mutex_);
  units_busy_.reset(static_cast<std::size_t>(unit));
  if (units_busy_.none() && nullptr != units_ready_cb_) {
    units_ready_cb_();
  }
}

void ObservableUnits::SetBusy(UnitId unit) {
  std::lock_guard<std::mutex> lock(mutex_);
  units_busy_.set(static_cast<std::size_t>(unit));
}

}  // namespace analyzer