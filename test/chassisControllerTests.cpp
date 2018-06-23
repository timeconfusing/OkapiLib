/**
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "test/tests/api/chassisControllerTests.hpp"
#include "test/crossPlatformTestRunner.hpp"
#include "okapi/api/chassis/controller/chassisScales.hpp"

void testChassisScales() {
  using namespace okapi;
  using namespace snowhouse;

  test_printf("Testing ChassisScales");

  {
    ChassisScales scales({0.5, 0.3});
    test("ChassisScales should accept raw scales", [&]() {
      AssertThat(scales.straight, Equals(0.5));
      AssertThat(scales.turn, Equals(0.3));
    });
  }

  {
    ChassisScales scales({4_in, 11.5_in});
    test("ChassisScales should calculate scales from wheelbase", [&]() {
      AssertThat(scales.straight, EqualsWithDelta(1127.86968, 0.0001));
      AssertThat(scales.turn, EqualsWithDelta(2.875, 0.001));
    });
  }
}
