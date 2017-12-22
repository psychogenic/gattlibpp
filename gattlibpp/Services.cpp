/*
 * Services.cpp
 *
 *  Created on: Dec 21, 2017
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

#include "../include/gattlibpp/BLECentralDebug.h"
#include "../include/gattlibpp/GattlibppExtInc.h"
#include "../include/gattlibpp/GattlibppTypes.h"


namespace Gattlib {




namespace Service {

UUID AnyService;

ServDetailsStruct::ServDetailsStruct() {
}


ServDetailsStruct::ServDetailsStruct(const ::Gattlib::UUID & devId,
		const UUID & servUUID,
		const gattlib_primary_service_t  & glservice) {
	deviceId = devId;
	id = servUUID;
	gl_service = glservice;
	gl_uuid = glservice.uuid;


}


}
}


