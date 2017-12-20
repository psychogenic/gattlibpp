/*
 * BLECentralDebug.h
 *
 *  Created on: Dec 18, 2017
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

#ifndef INCLUDES_BLECENTRALDEBUG_H_
#define INCLUDES_BLECENTRALDEBUG_H_

#include <iostream>

#include "../gattlibpp/BLECentralConfig.h"
#define BLECNTL_ERROR(...)		std::cerr << "ERROR:" << __VA_ARGS__
#define BLECNTL_ERRORLN(...)		std::cerr << "ERROR:" << __VA_ARGS__ << std::endl


#ifdef BLECENTRAL_DEBUG_ENABLE

#define BLECNTL_DEBUG(...)		std::cerr << __VA_ARGS__
#define BLECNTL_DEBUGLN(...)		std::cerr << __VA_ARGS__ << std::endl;

#else

#define BLECNTL_DEBUG(...)
#define BLECNTL_DEBUGLN(...)

#endif



#endif /* INCLUDES_BLECENTRALDEBUG_H_ */
