/*
 * GattlibppTypes.h
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

#ifndef INCLUDES_GATTLIBPPTYPES_H_
#define INCLUDES_GATTLIBPPTYPES_H_

#include "../gattlibpp/GattlibppExtInc.h"

namespace Gattlib {


typedef std::string AdapterName;
typedef void* AdapterPtr;

typedef std::string ErrorMessage;
typedef std::string UUID;
typedef UUID Service;

extern Service AnyService;

typedef std::vector<Service> ServiceList;

typedef uint16_t TimeValue;
typedef TimeValue SecondsValue;

typedef int8_t RSSIValue;

typedef std::vector<uint8_t> BinaryBuffer;
typedef BinaryBuffer AdvertisingBuffer;

namespace Discovery {

typedef struct DiscDeviceStruct{
	Service name;
	UUID id;
	RSSIValue rssi;
	AdvertisingBuffer advertising;

	DiscDeviceStruct() : rssi(0) {}

	DiscDeviceStruct(const UUID & uid, const Service & devName, RSSIValue rssiVal=0) :
		name(devName), id(uid), rssi(rssiVal) {

	}

} Device;


} /* Discovery */

namespace Callbacks {

typedef std::function<void()> QueuedOp;
typedef std::function<void(ErrorMessage err)> ErrorWithMessage;
typedef std::function<void()> ErrorNoParameter;
typedef ErrorNoParameter Error;
typedef std::function<void()> SuccessNotification;



typedef std::function<void(const Discovery::Device & dev)> Discovered;
typedef std::function<void(const BinaryBuffer & data)> IncomingData;
typedef std::function<void(const BinaryBuffer & data)> IncomingNotification;

typedef std::vector<QueuedOp> Queue;

} /* namespace Callbacks */


namespace Characteristic {
namespace Property {

typedef enum {
	Broadcast=0x01,
	Read=0x02,
	WriteWithoutResp=0x04,
	Write=0x08,
	Notify=0x10,
	Indicate=0x20,

	Undefined=0xff
} Value;

typedef std::set<Property::Value>	List;

} /* namespace Property */

typedef ::Gattlib::UUID		UUID;

typedef struct CharDetailsStruct {
	UUID 			id;
	::Gattlib::UUID	deviceId;
	Property::List	properties;
	bool			valid;
	bool 			registeredForNotifs;
	uuid_t			gl_uuid; // gattlib uuid

	CharDetailsStruct() ;
	CharDetailsStruct(const ::Gattlib::UUID & devId, const gattlib_characteristic_t  & char_desc) ;

	bool supports(Property::Value p) const;
	bool supportSubscriptions() const;
	bool supportsWrites() const;
	bool supportsReads() const;

} Details;

typedef std::vector<Details> List;

} /* namespace  Characteristic */

} /* namespace  Gattlib */


#endif /* INCLUDES_GATTLIBPPTYPES_H_ */
