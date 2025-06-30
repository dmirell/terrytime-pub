#include <gmock/gmock.h>

#include <memory>

#include "analyzer/scoped_unit_state.h"
#include "utils/tests/helpers/unit_state_mock.h"

namespace analyzer {

TEST(ScopedUnitState, GivenScopedUnitState_WhenEndScope_ThenSetBusyReadCalled) {
  // Given
  auto unit_state = std::make_shared<MockUnitState<ObservableUnits::UnitId::kAnalyzer>>();

  // Then
  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state, SetReady()).InSequence(unit_state_seq);

  ScopedUnitState<ObservableUnits::UnitId::kAnalyzer> scoped_state(unit_state);
}

}  // namespace analyzer