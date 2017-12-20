/*
 * DeviceDetails.h
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

#ifndef INCLUDES_DEVICEDETAILS_H_
#define INCLUDES_DEVICEDETAILS_H_

#include "../gattlibpp/BLECentralConfig.h"
#include "../gattlibpp/BLECentralDebug.h"
#include "../gattlibpp/GattlibppTypes.h"

namespace Gattlib {
namespace Device {

typedef std::map<Characteristic::UUID, Callbacks::IncomingNotification> NotificationHandlersMap;
typedef std::map<Characteristic::UUID, Characteristic::Details> CharacteristicsMap;

typedef struct DeviceDetailsStruct {

	DeviceDetailsStruct();
	DeviceDetailsStruct(const UUID & did);

	bool hasNotificationHandlerFor(const Characteristic::UUID & uuid);
	Callbacks::IncomingNotification notificationHandler(const Characteristic::UUID & uuid);
	void setNotificationHandler(const Characteristic::UUID & forUUID, Callbacks::IncomingNotification hndler);

	bool addCharacteristic(const Characteristic::UUID & uuid,
			const gattlib_characteristic_t & gl_char);

	bool hasCharacteristic(const Characteristic::UUID & uuid);

	Characteristic::Details & characteristic(const Characteristic::UUID & uuid);

	void foreachCharacteristic(std::function<void(Characteristic::Details &)> doOp);

	UUID id;
	gatt_connection_t* connection;
	bool discovery_done;
	uint8_t num_registered_for_notifs;

	CharacteristicsMap characteristics;
	NotificationHandlersMap notifHandlers;

#ifdef BLECENTRAL_THREADSAFE
	std::mutex conn_mutex;
#endif


} Details;


#ifdef BLECENTRAL_THREADSAFE

#define DEVDETS_LOCKGUARD_OPERATION(devdetsPtr, opname)		\
	BLECNTL_DEBUGLN("Attempting to lock device:" << opname);	\
	std::lock_guard<std::mutex> _guard(devdetsPtr->conn_mutex)

#define DEVDETS_LOCKGUARD_BEGINBLOCK(devdetsPtr, opname) \
		{ \
		DEVDETS_LOCKGUARD_OPERATION(devdetsPtr, opname)

#define DEVDETS_LOCKGUARD_ENDBLOCK() \
	BLECNTL_DEBUGLN("lock block end"); \
		}

#else
#define DEVDETS_LOCKGUARD_OPERATION()
#endif

}
}

#endif /* INCLUDES_DEVICEDETAILS_H_ */
