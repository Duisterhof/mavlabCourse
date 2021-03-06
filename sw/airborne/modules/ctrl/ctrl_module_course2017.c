/*
 * Copyright (C) 2015
 *
 * This file is part of Paparazzi.
 *
 * Paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * Paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Paparazzi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

/**
 * @file modules/ctrl/ctrl_module_course.h
 * @brief example empty controller
 *
 */

#include <stdbool.h>
#include "modules/ctrl/ctrl_module_course2017.h"
#include "modules/computer_vision/grass_detector.h"
#include "state.h"
#include "subsystems/navigation/waypoints.h"
#include "subsystems/radio_control.h"
#include "firmwares/rotorcraft/stabilization.h"
#include "firmwares/rotorcraft/stabilization/stabilization_attitude.h"
#include "firmwares/rotorcraft/guidance/guidance_v.h"

#include "generated/flight_plan.h"

#define BOUND_ANGLE 25.0
#define RAD_OF_DEG(d) (d*M_PI/180.)
#define angleBound(_x, _min, _max ) ( ( _x ) > ( _max ) ? ( _max ) : ( ( _x ) < ( _min ) ? ( _min ) : ( _x ) ) )

struct ctrl_module_course_struct {
  int rc_x;
  int rc_y;
  int rc_z;
  int rc_t;

} ctrl_module_course;

double rc_yaw_gain      = 0.05;
double rc_pitch_gain    = 0.35;
double rc_roll_gain     = 0.35;
double position_gain    = 1.0;

double settings_grass_gain = 70;

void ctrl_module_init(void);
void ctrl_module_run(bool in_flight);

void ctrl_module_init(void)
{
  ctrl_module_course.rc_x   = 0;
  ctrl_module_course.rc_y   = 0;
  ctrl_module_course.rc_z   = 0;
  ctrl_module_course.rc_t   = 0;
  guidance_v_z_sp           = POS_BFP_OF_REAL( -0.75 );
  guidance_v_zd_sp          = SPEED_BFP_OF_REAL( 0.0 );
}

struct Int32Eulers setpoint;

// simple rate control without reference model nor attitude
void ctrl_module_run(bool in_flight)
{
  double grass_gain = RAD_OF_DEG( settings_grass_gain );
  struct Int32Eulers * orientation = stateGetNedToBodyEulers_i();
  // Set the altitude and speed for vertical controller
  guidance_v_z_sp  = POS_BFP_OF_REAL( -0.75 );
  guidance_v_zd_sp = SPEED_BFP_OF_REAL( 0.0 );
  setpoint.psi     = orientation->psi;
  switch (cv_grass_detector.inside){
  case GRASS_UNSURE :
        // Keep same setpoints but rotate over change in yaw
        //cv_grass_detector.range *= 1.1;
      break;
  case GRASS_OUTSIDE :
      setpoint.theta    = ANGLE_BFP_OF_REAL( angleBound(-grass_gain * pow( cv_grass_detector.range, 2.0 ) * cos(cv_grass_detector.angle), RAD_OF_DEG(-BOUND_ANGLE), RAD_OF_DEG(BOUND_ANGLE) ) );
      setpoint.phi      = ANGLE_BFP_OF_REAL( angleBound(grass_gain * pow( cv_grass_detector.range, 2.0 ) * sin(cv_grass_detector.angle) , RAD_OF_DEG(-BOUND_ANGLE), RAD_OF_DEG(BOUND_ANGLE) ) );
      printf("theta: %4.2f phi: %4.2f\n", ANGLE_FLOAT_OF_BFP( setpoint.theta ) / M_PI * 180, ANGLE_FLOAT_OF_BFP( setpoint.phi ) / M_PI * 180);
      break;
  case GRASS_INSIDE :
      setpoint.phi      = 0; //ANGLE_BFP_OF_REAL( RAD_OF_DEG( 0.0 ) );
      setpoint.theta    = 0; //ANGLE_BFP_OF_REAL( RAD_OF_DEG( 0.0 ) );
      setpoint.phi     += rc_roll_gain * ctrl_module_course.rc_x;
      setpoint.theta   += rc_pitch_gain * ctrl_module_course.rc_y;
      setpoint.psi     += rc_yaw_gain * ctrl_module_course.rc_z;
      /*
      // Get our position and orientation
      struct EnuCoor_i*     position    = stateGetPositionEnu_i();
      struct Int32Eulers*   orientation = stateGetNedToBodyEulers_i();
      // Get the position of the waypoint we want to center at
      struct EnuCoor_i*     center      = &waypoints[WP_CENTER].enu_i;
      // Calculate the radial coordinates of the waypoint
      double centerAngle    = atan2(POS_FLOAT_OF_BFP(position->y - center->y), POS_FLOAT_OF_BFP(position->x - center->x));
      //double centerRadius   = hypot(POS_FLOAT_OF_BFP(position->x - center->x), POS_FLOAT_OF_BFP(position->y - center->y));
      // Determine the body angle to the waypoint
      double relativeAngle  = centerAngle - ANGLE_FLOAT_OF_BFP( orientation->psi );
      if(relativeAngle < -M_PI)     relativeAngle += 2*M_PI;
      if(relativeAngle >  M_PI)     relativeAngle -= 2*M_PI;
      // Now we put gains on the distances from the waypoint
      // Euler angles are derived from NED whilst waypoints are in ENU so we switch X and Y to derive our angles
      double xGain      = position_gain * POS_FLOAT_OF_BFP(position->y - center->y);
      double yGain      = position_gain * POS_FLOAT_OF_BFP(position->x - center->x);
      // And rotate these according to our relative waypoint
      // Our NED x and y gains should now be converted to body angles
      double dTheta     = cos(relativeAngle) * xGain - sin(relativeAngle) * yGain;
      double dPhi       = sin(relativeAngle) * xGain + cos(relativeAngle) * yGain;
      setpoint.theta    = (int32_t) ANGLE_BFP_OF_REAL( angleBound(dTheta, RAD_OF_DEG(-BOUND_ANGLE), RAD_OF_DEG(BOUND_ANGLE) ) );
      setpoint.phi      = (int32_t) ANGLE_BFP_OF_REAL( angleBound(dPhi,   RAD_OF_DEG(-BOUND_ANGLE), RAD_OF_DEG(BOUND_ANGLE) ) );
      // Allow the user to steer the drone using RC
      setpoint.theta   += (int32_t) round(ctrl_module_course.rc_y * rc_pitch_gain);
      setpoint.phi     += (int32_t) round(ctrl_module_course.rc_x * rc_roll_gain);
      setpoint.psi      = orientation->psi + ((int32_t) round(ctrl_module_course.rc_z * rc_yaw_gain));
      */
      break;
  }
  /*
  if (!in_flight) {
      // We aren't flying so reset the stabilization
      setpoint.phi      = ANGLE_BFP_OF_REAL( RAD_OF_DEG( 0.0 ) );
      setpoint.theta    = ANGLE_BFP_OF_REAL( RAD_OF_DEG( 0.0 ) );
  }
  */
  stabilization_attitude_set_rpy_setpoint_i(&setpoint);
  stabilization_attitude_run(in_flight);
}


////////////////////////////////////////////////////////////////////
// Call our controller
// Implement own Horizontal loops
void guidance_h_module_init(void)
{
  ctrl_module_init();
}

void guidance_h_module_enter(void)
{
  ctrl_module_init();
}

void guidance_h_module_read_rc(void)
{
  // -MAX_PPRZ to MAX_PPRZ
  ctrl_module_course.rc_t = radio_control.values[RADIO_THROTTLE];
  ctrl_module_course.rc_x = radio_control.values[RADIO_ROLL];
  ctrl_module_course.rc_y = radio_control.values[RADIO_PITCH];
  ctrl_module_course.rc_z = radio_control.values[RADIO_YAW];
}

void guidance_h_module_run(bool in_flight)
{
  // Call full inner-/outerloop / horizontal-/vertical controller:
  ctrl_module_run(in_flight);
}

/*
void guidance_v_module_init(void)
{
  // initialization of your custom vertical controller goes here
}

// Implement own Vertical loops
void guidance_v_module_enter(void)
{
  // your code that should be executed when entering this vertical mode goes here
}

void guidance_v_module_run(__attribute__((unused)) bool in_flight)
{
  // your vertical controller goes here
}
*/
