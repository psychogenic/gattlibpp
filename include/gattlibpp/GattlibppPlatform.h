/*
 * GattlibppPlatform.h
 *
 *  Created on: Dec 19, 2017
 *        Copyright (C) 2017 Pat Deegan, https://psychogenic.com
 *  This file is part of GattLib++.
 *
 *  GattLib++ is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *  GattLib++ is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GattLib++.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDES_GATTLIBPPPLATFORM_H_
#define INCLUDES_GATTLIBPPPLATFORM_H_


#include "../gattlibpp/GattlibppExtInc.h"

#ifdef BLECENTRAL_PLATFORM_UNIX
#include <unistd.h>
#define BLECNTL_SLEEP_US(us)		usleep(us)
#else
#define BLECNTL_SLEEP_US(us)
#endif



#endif /* INCLUDES_GATTLIBPPPLATFORM_H_ */
