#include <gmock/gmock.h>

#include <cctype>
#include <chrono>

#include "utils/time/global_clock.h"
#include "utils/time/i_time_provider.h"
#include "utils/time/time_provider.h"

namespace {
class FakeGlobalClock : public utils::GlobalClock {
 public:
  std::shared_ptr<utils::ITimeProvider> time_provider() const { return time_provider_; }
};

class ITimeProviderMock : public utils::ITimeProvider {
 public:
  MOCK_METHOD(void, WaitUntil, (utils::Timestamp), (override));
  MOCK_METHOD(utils::Timestamp, Now, (), (const override));
};
}  // namespace

TEST(GlobalClock, GivenFakeGlobalClock_WhenCreate_ThenDefaultToRegularTimeProvider) {
  FakeGlobalClock clock;
  EXPECT_EQ(typeid(utils::TimeProvider), typeid(*clock.time_provider()));
}

TEST(GlobalClock, GivenFakeGlobalClock_WhenSetTimeProvider_ThenSet) {
  FakeGlobalClock clock;
  auto mock_i_time_provider = std::make_shared<ITimeProviderMock>();
  clock.SetTimeProvider(mock_i_time_provider);
  EXPECT_EQ(typeid(ITimeProviderMock), typeid(*clock.time_provider()));
}

TEST(GlobalClock, GivenFakeGlobalClock_WhenCallNow_ThenNowTimeProviderCalled) {
  FakeGlobalClock clock;
  auto mock_i_time_provider = std::make_shared<ITimeProviderMock>();
  clock.SetTimeProvider(mock_i_time_provider);

  EXPECT_CALL(*mock_i_time_provider, Now());
  clock.Now();
}

TEST(GlobalClock,
     GivenFakeGlobalClock_WhenCallWaitUntil_ThenWaitUntilTimeProviderCalled) {
  FakeGlobalClock clock;
  auto mock_i_time_provider = std::make_shared<ITimeProviderMock>();
  clock.SetTimeProvider(mock_i_time_provider);

  EXPECT_CALL(*mock_i_time_provider, WaitUntil(0));
  clock.WaitUntil(0);
}
