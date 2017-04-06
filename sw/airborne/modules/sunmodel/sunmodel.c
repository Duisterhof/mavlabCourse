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
 * @file "modules/sunmodel/sunmodel.c"
 * @author BD
 * Find optimum charging spot for solar array powered UAV's
 */

#include <stdlib.h>
#include <stdio.h>
#include "std.h" //For printf debugging

#include "modules/sunmodel/sunmodel.h"
#include "subsystems/abi.h"
//#include "subsystems/datalink/datalink.h"

#ifndef SUNMODEL_SENDER_ID
#define SUNMODEL_SENDER_ID 1
#endif

bool suncalc_once;

void hefysuncalc(void);
void sunmodel_init(void);
void sunmodel_event(void);
void sunmodel_periodic(void);

void sunmodel_init(void)
{
	//here init stuff
	suncalc_once = false;
	printf("init now:\n");
	// do the dgsdgsdg
	hefysuncalc();
	printf("done now:\n");
}

void sunmodel_periodic(void)
{
	//
}


void sunmodel_event(void)
{
	//do something
	if (suncalc_once) {

		printf("calcone !\n");
		suncalc_once = false;
	}
}

void sunmodel_dl_callback(void)
{
	//
}

void hefysuncalc(void)
{
	sleep(6);
}


