/*
 * Copyright (C) BD
 *
 * This file is part of paparazzi
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * @file "modules/sunmodel/sunmodel.h"
 * @author BD
 * Find optimum charging spot for solar array powered UAV's
 */

#include "stdbool.h"

#ifndef SUNMODEL_H
#define SUNMODEL_H

extern void sunmodel_init(void);
extern void sunmodel_event(void);
extern void sunmodel_periodic(void);
extern void sunmodel_dl_callback(void);

extern bool suncalc_once;

#endif

