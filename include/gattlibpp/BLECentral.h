/*
 * BLECentral.h
 *
 *  Created on: Dec 16, 2017
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

#ifndef INCLUDES_BLECENTRAL_H_
#define INCLUDES_BLECENTRAL_H_
#include "../gattlibpp/DeviceDetails.h"
#include "../gattlibpp/GattlibppTypes.h"

#include <queue>
#include <list>
namespace Gattlib {


typedef struct CallbackPairStruct {
	Callbacks::SuccessNotification success;
	Callbacks::Error error;
	CallbackPairStruct(Callbacks::SuccessNotification & succ,
			Callbacks::Error & err ) : success(succ), error(err)
	{

	}
}CallbackPair ;

typedef std::vector<CallbackPair> CallbackStack;

typedef struct CallbacksContainerStruct {
	Callbacks::SuccessNotification success;
	Callbacks::Error failure;
	Callbacks::Discovered	discovered;
	Callbacks::IncomingData datareceived;



	CallbacksContainerStruct() : success(NULL), failure(NULL), discovered(NULL), datareceived(NULL) {}

	void clear() {
		success = NULL;
		failure = NULL;
		discovered = NULL;
		datareceived = NULL;

	}

	void setup(Callbacks::SuccessNotification & succ, Callbacks::Error & err) {
		success = succ;
		failure = err;
	}
/*
	void setup(Callbacks::SuccessNotification & succ, Callbacks::Error & err, Callbacks::Discovered & disc) {
		setup(succ, err);
		discovered = disc;
	}
*/

} CallbacksContainer;

#define BDADDR_BREDR           0x00
#define BDADDR_LE_PUBLIC       0x01
#define BDADDR_LE_RANDOM       0x02

namespace Address {
typedef enum AddrType {
	BREDR = 0,
	LE_PUBLIC=0x01,
	LE_RANDOM=0x02
}Type;
}

typedef struct ConnectionParamsStruct {
	Address::Type	destinationType;
	gattlib_bt_sec_level_t security;
	int PSM;
	int MTU;
	ConnectionParamsStruct() : destinationType(Address::LE_PUBLIC),
			security(BT_SEC_LOW), PSM(0), MTU(0) {}

} ConnectionParams;


typedef std::map<UUID, Device::Details> 	DeviceMap;
typedef std::map<UUID, DeviceName>			DeviceNamesMap;

namespace AsyncAction {

namespace State {
typedef enum StateValueEnum {
	Pending,
	Triggered,
	Completed
}Value;
}

// forward decl
struct AsyncActionDetailsStruct;
typedef std::function<void(AsyncActionDetailsStruct * thisAction)> TriggerFunction;

typedef struct AsyncActionDetailsStruct {
	std::string name;
	State::Value state;
	CallbacksContainer callbacks;
	TriggerFunction trigger;
	uint16_t waitCount;

	AsyncActionDetailsStruct() :
			state(State::Pending), trigger(NULL), waitCount(0) {

	}
	AsyncActionDetailsStruct(const std::string & actionName, const TriggerFunction & trig,
			const Callbacks::SuccessNotification & succ,
			const Callbacks::Error & fail) :
			name(actionName),
			state(State::Pending), trigger(trig), waitCount(0) {
		callbacks.success = succ;
		callbacks.failure = fail;

	}
	AsyncActionDetailsStruct(const std::string & actionName,
			const Callbacks::SuccessNotification & succ,
			const Callbacks::Error & fail) :
				name(actionName),
				state(State::Pending),
				trigger(NULL), waitCount(0) {

		callbacks.success = succ;
		callbacks.failure = fail;
	}
	AsyncActionDetailsStruct(const std::string & actionName,
			const Callbacks::Discovered & discovered,
			const Callbacks::Error & fail) :
				name(actionName),
				state(State::Pending),
				trigger(NULL), waitCount(0) {

		callbacks.discovered = discovered;
		callbacks.failure = fail;
	}

	AsyncActionDetailsStruct(const std::string & actionName,
			const Callbacks::IncomingData & inData,
			const Callbacks::Error & fail) :
				name(actionName),
				state(State::Pending),
				trigger(NULL), waitCount(0) {

		callbacks.datareceived = inData;
		callbacks.failure = fail;
	}

	bool run() {
		if (!isPending()) {
			return false;
		}

		state = State::Triggered;
		trigger(this);
		return true;
	}

	bool isPending() const {
		return state == State::Pending;
	}

	bool isRunning() const {
		return state == State::Triggered;
	}

	bool isDone() const {
		return state == State::Completed;
	}

	void incrementWaitCount() {
		waitCount++;
	}

	Callbacks::SuccessNotification successWrapper() {
		auto wrap = [this]() {
			this->completedWithSuccess();
		};

		return wrap;
	}
	Callbacks::Error failWrapper() {
		auto wrap = [this]() {
			this->completedWithFailure();
		};

		return wrap;
	}


	// used to terminate this asyn action
	void cancelWithoutCallback() {
		markCompleted();
	}
	inline void markCompleted() {
		state = State::Completed;
	}
	void completedWithSuccess() {
		markCompleted();
		if (callbacks.success) {
			(callbacks.success)();
		}
	}

	void completedWithFailure() {
		markCompleted();
		if (callbacks.failure) {
			(callbacks.failure)();
		}
	}

} Details;

}

typedef std::queue<AsyncAction::Details>				AsyncActionQueue;

class BLECentral {
public:
	static BLECentral * getInstance();

	virtual ~BLECentral();


	/* basic setup... on start, you may select an
	 * adapter to open, e.g. hci0, or just use the default
	 * by ignoring these.
	 */
	bool adapterOpen(const AdapterName & name="");
	bool adapterClose();
	bool adapterIsOpen() { return adapter_is_open;}


	/*
	 * when exiting the program, shutdown() will
	 * handle closing an open connection, and the adapter.
	 */
	void shutdown();

	/*
	 * during use, regular calls to processAsync() will
	 * tick the system an enable event processing and
	 * callback triggers.
	 */
	void processAsync();
	inline void tick() { processAsync(); }

	// configuration

	// autoDiscoverOnConnect -- whether characteristics are read/cached
	// on successful connect.
	bool autoDiscoverOnConnect() { return auto_discover;}
	void setAutoDiscoverOnConnect(bool setTo) { auto_discover = setTo;}

	// connection params to use when opening connections
	inline ConnectionParams & connectionParameters() { return connParams;}


	/* ************* BLUETOOTH Operations ************* */


	// scanning

	/*
	 * scan -- starts a scan and with a specified timeout (or until stopScan)
	 */
	bool scan(const Service::UUIDList & services, SecondsValue runSeconds,
			Callbacks::SuccessNotification scanCompleted,
			Callbacks::Discovered deviceDiscoveredCb, Callbacks::Error failure=NULL);


	bool scan(SecondsValue runSeconds,
			Callbacks::SuccessNotification scanCompleted,
			Callbacks::Discovered deviceDiscoveredCb, Callbacks::Error failure=NULL);

	/*
	 * startScan -- will scan until stopScan is called.
	 */

	bool startScan(Callbacks::SuccessNotification scanCompleted,
			Callbacks::Discovered deviceDiscoveredCb, Callbacks::Error failure=NULL);

	bool startScan(const Service::UUIDList & services,
			Callbacks::SuccessNotification scanCompleted,
			Callbacks::Discovered deviceDiscoveredCb, Callbacks::Error failure=NULL);

	/*
	 * stopScan -- what it says on the box.
	 */
	bool stopScan(Callbacks::SuccessNotification succ=NULL, Callbacks::Error failure=NULL);


	/*
	 * connecting to a device
	 */
	bool connect(const UUID & device, Callbacks::SuccessNotification succ, Callbacks::Error failure=NULL);
	bool connect(const UUID & device, const ConnectionParams & parameters, Callbacks::SuccessNotification succ, Callbacks::Error failure=NULL);
	bool isConnected(const UUID & devId);
	bool disconnect(const UUID & device, Callbacks::SuccessNotification succ=NULL,
			Callbacks::Error failure=NULL);


	Service::List servicesFor(const UUID & device);
	Characteristic::List characteristicsFor(const UUID & device);


	bool read(const UUID & device, const Service::UUID & service_uuid,
			const Characteristic::UUID & characteristic_uuid,
			Callbacks::IncomingData inData,
						Callbacks::Error failure=NULL);

	bool read(const UUID & device,
			const Characteristic::UUID & characteristic_uuid,
			Callbacks::IncomingData inData,
						Callbacks::Error failure=NULL);


	bool write(const UUID & device,
			const Characteristic::UUID & characteristic_uuid,
			const BinaryBuffer & value,
			Callbacks::SuccessNotification succ=NULL,
						Callbacks::Error failure=NULL);

	bool write(const UUID & device,
			const Service::UUID & service_uuid,
			const Characteristic::UUID & characteristic_uuid,
			const BinaryBuffer & value,
			Callbacks::SuccessNotification succ=NULL,
						Callbacks::Error failure=NULL);


	bool writeWithoutResponse(const UUID & device,
			const Characteristic::UUID & characteristic_uuid,
			const BinaryBuffer & value,
			Callbacks::SuccessNotification succ=NULL,
						Callbacks::Error failure=NULL);

	bool writeWithoutResponse(const UUID & device, const Service::UUID & service_uuid,
			const Characteristic::UUID & characteristic_uuid,
			const BinaryBuffer & value,
			Callbacks::SuccessNotification succ=NULL,
						Callbacks::Error failure=NULL);

	bool startNotification(const UUID & device, const Service::UUID & service_uuid,
			const Characteristic::UUID & characteristic_uuid, Callbacks::IncomingData inDataCallback,
			Callbacks::Error failure=NULL);


	bool stopNotification(const UUID & device, const Service::UUID & service_uuid,
			const Characteristic::UUID & characteristic_uuid,
			Callbacks::SuccessNotification succ=NULL,
						Callbacks::Error failure=NULL);

	void stopAllNotifications(const UUID & device);


	bool isConnected(const UUID & device, Callbacks::SuccessNotification succ,
						Callbacks::Error failure);
	bool isEnabled(Callbacks::SuccessNotification succ,
						Callbacks::Error failure);

	bool enable(Callbacks::SuccessNotification succ=NULL,
						Callbacks::Error failure=NULL);
	/*

    showBluetoothSettings: function () {
        return new Promise(function(resolve, reject) {
            module.exports.showBluetoothSettings(resolve, reject);
        });
    },

    stopStateNotifications: function () {
        return new Promise(function(resolve, reject) {
            module.exports.stopStateNotifications(resolve, reject);
        });




        readRSSI: function(device_id) {
            return new Promise(function(resolve, reject) {
                module.exports.readRSSI(device_id);
            });
        }

        */
/*
	gatt_connection_t *gattlib_connect(const char *src, const char *dst,
					uint8_t dest_type, gattlib_bt_sec_level_t sec_level, int psm, int mtu);

*/


	const DeviceName & deviceName(const UUID & devUUID);


	// ********** internal usage **************

	/* notifications from callbacks etc */
	void currentOpCompleted();
	void currentOpFailed();
	void deviceConnected(gatt_connection_t* connection);
	void deviceDisconnected();
	void deviceDiscovered(const Discovery::Device & dev);
	void scanCompleted();
	void scanDisableCompleted();
	void readDataReceived(const uint8_t * buf, size_t len);
	void handleNotification(const Characteristic::UUID & charUUID,
								const uint8_t* data,
								size_t data_length);


private:
	static BLECentral * ble_central_singleton;

	BLECentral();
	bool openAdapterIfRequired();
	Device::Details * deviceDetails(const UUID & devId);

	bool performDiscovery(Device::Details * onDevice);
	bool performServiceDiscovery(Device::Details * onDevice);
	bool performCharacteristicsDiscovery(Device::Details * onDevice);

	void autoDiscoverServicesStep(Device::Details * onDevice);
	void autoDiscoverCharacteristicsStep(Device::Details * onDevice);
	void queueAsyncAction(const AsyncAction::Details & dets);


	AdapterPtr adapter;
	AdapterName adapter_name;
	DeviceMap devices;
	bool adapter_is_open;
	bool is_scanning;
	bool auto_discover;
	UUID connected_to;

	ConnectionParams connParams;

	DeviceNamesMap namesCache;

	Callbacks::Discovered devDiscoveredCb;


	AsyncActionQueue asyncActions;
	Callbacks::Queue reportQueue;


};

} /* namespace Gattlibpp */

#endif /* INCLUDES_BLECENTRAL_H_ */
