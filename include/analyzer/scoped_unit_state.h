#ifndef INCLUDE_ANALYZER_SCOPED_UNIT_STATE_H_
#define INCLUDE_ANALYZER_SCOPED_UNIT_STATE_H_

#include <cassert>
#include <memory>

#include "analyzer/observable_units.h"

namespace analyzer {

template <ObservableUnits::UnitId unit>
class ScopedUnitState {
 public:
  ScopedUnitState() = delete;
  ScopedUnitState(const ScopedUnitState &) = delete;
  ScopedUnitState(ScopedUnitState &&) = delete;
  ScopedUnitState &operator=(const ScopedUnitState &) = delete;
  ScopedUnitState &operator=(ScopedUnitState &&) = delete;

  ScopedUnitState(const std::shared_ptr<ObservableUnits::Unit<unit>> &unit_state)
      : unit_state_(unit_state) {
    assert(nullptr != unit_state_);
    unit_state_->SetBusy();
  }

  ~ScopedUnitState() { unit_state_->SetReady(); }

 private:
  std::shared_ptr<ObservableUnits::Unit<unit>> unit_state_;
};
}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_SCOPED_UNIT_STATE_H_