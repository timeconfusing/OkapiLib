/**
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "okapi/api/chassis/controller/chassisScales.hpp"
#include "okapi/api/chassis/model/readOnlyChassisModel.hpp"
#include "okapi/api/units/QSpeed.hpp"
#include "okapi/api/util/abstractRate.hpp"
#include "okapi/api/util/logging.hpp"
#include "okapi/api/util/timeUtil.hpp"
#include <atomic>
#include <memory>
#include <valarray>

namespace okapi {
struct OdomState {
  QLength x{0_m};
  QLength y{0_m};
  QAngle theta{0_deg};

  bool operator==(const OdomState &rhs) const;

  bool operator!=(const OdomState &rhs) const;
};

/**
 * The mode for the state calculated by odometry.
 * FRAME_TRANSFORMATION: +x is forward, +y is right
 * CARTESIAN: +x is right, +y is forward
 */
enum class StateMode { FRAME_TRANSFORMATION, CARTESIAN };

class Odometry {
  public:
  /**
   * Odometry. Tracks the movement of the robot and estimates its position in coordinates
   * relative to the start (assumed to be (0, 0, 0)).
   *
   * @param itimeUtil The TimeUtil.
   * @param imodel The chassis model for reading sensors.
   * @param ichassisScales The chassis dimensions.
   * @param iwheelVelDelta The maximum delta between wheel velocities to consider the robot as
   * driving straight.
   * @param ilogger The logger this instance will log to.
   */
  Odometry(const TimeUtil &itimeUtil,
           const std::shared_ptr<ReadOnlyChassisModel> &imodel,
           const ChassisScales &ichassisScales,
           const QSpeed &iwheelVelDelta = 0.0001_mps,
           const std::shared_ptr<Logger> &ilogger = std::make_shared<Logger>());

  virtual ~Odometry() = default;

  /**
   * Sets the drive and turn scales.
   */
  virtual void setScales(const ChassisScales &ichassisScales);

  /**
   * Do one odometry step.
   */
  virtual void step();

  /**
   * Returns the current state.
   *
   * @param imode The mode to return the state in.
   * @return current state
   */
  virtual OdomState getState(const StateMode &imode = StateMode::FRAME_TRANSFORMATION) const;

  /**
   * Sets a new state to be the current state.
   *
   * @param istate new state
   */
  virtual void setState(const OdomState &istate);

  protected:
  std::shared_ptr<Logger> logger;
  std::unique_ptr<AbstractRate> rate;
  std::unique_ptr<AbstractTimer> timer;
  std::shared_ptr<ReadOnlyChassisModel> model;
  ChassisScales chassisScales;
  QSpeed wheelVelDelta;
  OdomState state;
  std::valarray<std::int32_t> newTicks{0, 0, 0}, tickDiff{0, 0, 0}, lastTicks{0, 0, 0};

  /**
   * Does the math, side-effect free, for one odom step.
   *
   * @param tickDiff The tick difference from the previous step to this step.
   * @param deltaT The time difference from the previous step to this step.
   * @return The estimated position/orientation offset, sinTheta, cosTheta.
   */
  virtual OdomState odomMathStep(std::valarray<std::int32_t> &tickDiff, const QTime &deltaT);
};
} // namespace okapi
