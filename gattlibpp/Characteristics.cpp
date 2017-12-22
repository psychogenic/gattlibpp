/*
 * Characteristics.cpp
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
#include "../include/gattlibpp/BLECentralDebug.h"
#include "../include/gattlibpp/GattlibppExtInc.h"
#include "../include/gattlibpp/GattlibppTypes.h"


namespace Gattlib {


namespace Characteristic {
CharDetailsStruct::CharDetailsStruct() : valid(false), registeredForNotifs(false) {

}
CharDetailsStruct::CharDetailsStruct(const ::Gattlib::UUID & devId,
		const UUID & charUUID,
		const gattlib_characteristic_t  & char_desc) :
		deviceId(devId),
		valid(false),
		registeredForNotifs(false)
{
	// char cuuid[BLECENTRAL_UUIDSTRING_MAX_LENGTH + 1] = {0,};
	gl_uuid = char_desc.uuid;
	/*
	if ( gattlib_uuid_to_string(&(gl_uuid), cuuid, BLECENTRAL_UUIDSTRING_MAX_LENGTH) != 0) {
		return;
	}
	*/

	id = charUUID;
	BLECNTL_DEBUGLN("Constructed char " << id << " with props " << (int)char_desc.properties);
	Property::Value possVals[] = {

			Property::Broadcast,
			Property::Read,
			Property::WriteWithoutResp,
			Property::Write,
			Property::Notify,
			Property::Indicate,

			Property::Undefined
	};
	uint8_t i=0;
	while (possVals[i] != Property::Undefined) {
		if (char_desc.properties & (uint8_t)(possVals[i])) {
			properties.insert(possVals[i]);
		}
		i++;
	}


	valid = true;

}

bool CharDetailsStruct::supports(Property::Value p) const {
	return properties.find(p) != properties.end();

}


bool CharDetailsStruct::supportSubscriptions() const {
	return supports(Property::Notify) || supports(Property::Indicate);

}
bool CharDetailsStruct::supportsWrites() const {
	return supports(Property::Write) || supports(Property::WriteWithoutResp);
}
bool CharDetailsStruct::supportsReads() const {
	return supports(Property::Read);
}


}



}



