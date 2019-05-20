/*
 * DeviceDetails.cpp
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
#include "../include/gattlibpp/DeviceDetails.h"

#include <algorithm>
#include <string>

namespace Gattlib {

namespace Device {

/* normalize*
 * ensure upper/lowercase doesn't cause us to miss
 * the fact that we've actually seen this service/characteristic
 */
static Characteristic::UUID normalizeCharacteristicUUID(const Characteristic::UUID& characteristic_uuid) {
	
	Characteristic::UUID charuid = characteristic_uuid;
	std::transform(charuid.begin(), charuid.end(), charuid.begin(), ::tolower);
	BLECNTL_DEBUGLN("NORMALIZED " << characteristic_uuid << " TO " << charuid);
	return charuid;
}

static Service::UUID normalizeServiceUUID(const Service::UUID& service_uuid) {
	Service::UUID suid = service_uuid;
	std::transform(suid.begin(), suid.end(), suid.begin(), ::tolower);
	BLECNTL_DEBUGLN("NORMALIZED " << service_uuid << " TO " << suid);
	return suid;
}

DeviceDetailsStruct::DeviceDetailsStruct() : connection(NULL), discovery_done(false),
		num_registered_for_notifs(0){

}
DeviceDetailsStruct::DeviceDetailsStruct(const UUID & did) : id(did), connection(NULL),
		discovery_done(false), num_registered_for_notifs(0) {

}

bool Details::hasNotificationHandlerFor(const Characteristic::UUID & uuid) {
	return notifHandlers.find(normalizeCharacteristicUUID(uuid)) != notifHandlers.end();
}
Callbacks::IncomingNotification Details::notificationHandler(const Characteristic::UUID & uuid) {
	return (*(notifHandlers.find(normalizeCharacteristicUUID(uuid)))).second;
}


bool Details::addService(const Service::UUID & uuid,
		const gattlib_primary_service_t & gl_serv) {
		Service::UUID suid = normalizeServiceUUID(uuid);
		Service::Details sd(id, suid, gl_serv);

		services.insert(ServicesMap::value_type(suid, sd));
		return true;

}
void Details::clearServices() {
	services.clear();
}

bool Details::hasService(const Service::UUID & uuid) {
	return services.find(normalizeServiceUUID(uuid)) != services.end();
}
Service::Details & Details::service(const Service::UUID & uuid) {
	ServicesMap::iterator iter = services.find(normalizeServiceUUID(uuid));
	return (*iter).second;
}
void Details::foreachService(std::function<void(Service::Details &)> doOp) {

	for (ServicesMap::iterator iter = services.begin(); iter != services.end();
			iter++)
	{
		doOp((*iter).second);
	}
}

void Details::clearCharacteristics(){
	characteristics.clear();
}
void Details::foreachCharacteristic(std::function<void(Characteristic::Details &)> doOp) {

	for (CharacteristicsMap::iterator iter = characteristics.begin(); iter != characteristics.end();
			iter++)
	{
		doOp((*iter).second);
	}
}
bool Details::addCharacteristic(const Characteristic::UUID & uuid,
		const gattlib_characteristic_t & gl_char) {

	Characteristic::UUID cuid = normalizeCharacteristicUUID(uuid);

	Characteristic::Details cd(id, cuid, gl_char);

	if (cd.valid) {
		characteristics.insert(CharacteristicsMap::value_type(cuid, cd));
		return true;
	}

	return false;
}

bool Details::hasCharacteristic(const Characteristic::UUID & uuid) {
	return characteristics.find(normalizeCharacteristicUUID(uuid)) != characteristics.end();
}

Characteristic::Details & Details::characteristic(const Characteristic::UUID & uuid) {
	CharacteristicsMap::iterator iter = characteristics.find(normalizeCharacteristicUUID(uuid));
	return (*iter).second;
}

void Details::setNotificationHandler(const Characteristic::UUID & forUUID, Callbacks::IncomingNotification hndler) {
	notifHandlers[normalizeCharacteristicUUID(forUUID)] = hndler;
}



}
}


