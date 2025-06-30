#ifndef INCLUDE_UTILS_TESTS_HELPERS_UNIT_STATE_MOCK_H_
#define INCLUDE_UTILS_TESTS_HELPERS_UNIT_STATE_MOCK_H_

#include <gmock/gmock.h>

#include "analyzer/observable_units.h"

template <analyzer::ObservableUnits::UnitId unit>
class MockUnitState : public analyzer::ObservableUnits::Unit<unit> {
 public:
  MockUnitState() : analyzer::ObservableUnits::Unit<unit>() {}

  MOCK_METHOD(void, SetBusy, (), (override));
  MOCK_METHOD(void, SetReady, (), (override));
};

#endif  // INCLUDE_UTILS_TESTS_HELPERS_UNIT_STATE_MOCK_H_