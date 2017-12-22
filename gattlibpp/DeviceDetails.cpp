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

namespace Gattlib {

namespace Device {

DeviceDetailsStruct::DeviceDetailsStruct() : connection(NULL), discovery_done(false),
		num_registered_for_notifs(0){

}
DeviceDetailsStruct::DeviceDetailsStruct(const UUID & did) : id(did), connection(NULL),
		discovery_done(false), num_registered_for_notifs(0) {

}

bool Details::hasNotificationHandlerFor(const Characteristic::UUID & uuid) {
	return notifHandlers.find(uuid) != notifHandlers.end();
}
Callbacks::IncomingNotification Details::notificationHandler(const Characteristic::UUID & uuid) {
	return (*(notifHandlers.find(uuid))).second;
}


bool Details::addService(const Service::UUID & uuid,
		const gattlib_primary_service_t & gl_serv) {
		Service::Details sd(id, uuid, gl_serv);

		services.insert(ServicesMap::value_type(uuid, sd));
		return true;

}
void Details::clearServices() {
	services.clear();
}

bool Details::hasService(const Service::UUID & uuid) {
	return services.find(uuid) != services.end();
}
Service::Details & Details::service(const Service::UUID & uuid) {
	ServicesMap::iterator iter = services.find(uuid);
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
	Characteristic::Details cd(id, uuid, gl_char);

	if (cd.valid) {
		characteristics.insert(CharacteristicsMap::value_type(uuid, cd));
		return true;
	}

	return false;
}

bool Details::hasCharacteristic(const Characteristic::UUID & uuid) {
	return characteristics.find(uuid) != characteristics.end();
}

Characteristic::Details & Details::characteristic(const Characteristic::UUID & uuid) {
	CharacteristicsMap::iterator iter = characteristics.find(uuid);
	return (*iter).second;
}

void Details::setNotificationHandler(const Characteristic::UUID & forUUID, Callbacks::IncomingNotification hndler) {
	notifHandlers[forUUID] = hndler;
}



}
}


