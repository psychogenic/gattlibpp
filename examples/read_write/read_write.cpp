/*
 * read_write.cpp
 *
 *  Created on: Dec 18, 2017
 *
 *  Copyright (C) 2017 Pat Deegan, https://psychogenic.com
 *
 *  Usage:
 *    read_write <device_address> <read|write> <uuid> [<hex-value-to-write>]
 *
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

#include <Gattlibpp.h>
#include <iostream>
#include <unistd.h> // for usleep

#define ARG_DEV_MINLENGTH		10
#define ARG_UUID_MINLENGTH		4

#define MSG_OUT(...) std::cout <<  __VA_ARGS__
#define MSG_OUTLN(...) std::cout << std::endl <<  __VA_ARGS__  << std::endl;

#define DBG_ENABLED

#ifdef DBG_ENABLED
#define DBG_OUT(...)	std::cerr <<  __VA_ARGS__
#define DBG_OUTLN(...)	std::cerr << std::endl << __VA_ARGS__ << std::endl
#else
#define DBG_OUT(...)
#define DBG_OUTLN(...)
#endif

typedef enum opTypeEnum {
	READ_OP, WRITE_OP
} OpType;

static void usage(char *argv[]) {
	MSG_OUTLN(
			argv[0] << " <device_address> <read|write> <uuid> [<hex-value-to-write>]")
}

long int getHex(const std::string & hexstr) {
	if (hexstr.size() > 2 && hexstr[0] == '0' && hexstr[1] == 'x') {

		return strtol(hexstr.c_str(), NULL, 0);
	}

	return strtol(hexstr.c_str(), 0, 16);
}

// forward decl
void useEnabledBLECentral(const std::string & device, const std::string & uuid,
		OpType selectedOp, long int hexval);

bool doContinue = true; // global flag to tell us when to quit

void failedCallback() {
	MSG_OUTLN("Last operation failed!  we're outta here.");
	doContinue = false;
}

int main(int argc, char *argv[]) {
	std::string op, device, uuid;
	OpType selectedOp(READ_OP);
	long int hexval = 0;

	// parse arguments
	if ((argc != 4) && (argc != 5)) {
		usage(argv);
		return 1;
	}
	op = argv[2];
	if (op == "read") {
		selectedOp = READ_OP;
	} else if (op == "write" && (argc == 5)) {
		selectedOp = WRITE_OP;
		hexval = getHex(argv[4]);
		MSG_OUTLN("Value to write: 0x" << std::hex << hexval);
	} else {
		usage(argv);
		return 1;
	}

	device = argv[1];
	uuid = argv[3];
	if ((device.size() < ARG_DEV_MINLENGTH) || uuid.size() < ARG_UUID_MINLENGTH) {
		usage(argv);
		return 2;
	}


	// get down to business

	Gattlib::BLECentral::getInstance()->enable(
		// enable worked out:
		[selectedOp, device, uuid, hexval]() {
				MSG_OUTLN("Device enabled");
				// ready to go! this will start up a chain of operations
				useEnabledBLECentral(device, uuid, selectedOp, hexval);
			},
		// enable failed...
		[]() {
				MSG_OUTLN("Could not even enable device, such sadness.");
				MSG_OUTLN("Check that bluetooth is up & running and that we can actually use it.");
				doContinue = false;

		});

	// just loop here... this is where you could handle
	// UI and other tasks.  The important thing is just
	// that we need to periodically call processAsync()
	// so we can handle BLE events and hand out time to
	// appropriate callbacks.
	while (doContinue) {
		// our processAsync() call
		Gattlib::BLECentral::getInstance()->processAsync();

		// whatever else we want to do... here we'll
		// just sleep
		usleep(50000);
		std::cerr << ".";
	}

	// errors or the end of the line have let us out
	// of our main loop... do call shutdown() in order
	//

	DBG_OUTLN("Doing shutdown");
	Gattlib::BLECentral::getInstance()->shutdown();

	return 0;

}

void useEnabledBLECentral(const std::string & device, const std::string & uuid,
		OpType selectedOp, long int hexval) {

	/*
	 * When using the lib, you use callbacks to get responses/status when
	 * ready.
	 *
	 * This can be done with actual functions, or for quick/short ops, a
	 * lambda is a nice way to do it, as demoed here, but it can get
	 * confusing when you're in too deep... even this example may be pushing it...
	 *
	 * In any case, here we'll use lambdas to connect and, on success, either
	 * read or write to the device, then provide feedback in respective callbacks.
	 */

	Gattlib::BLECentral* central = Gattlib::BLECentral::getInstance();
	central->connectionParameters().security = BT_SEC_LOW;
	central->connectionParameters().destinationType =
			Gattlib::Address::LE_PUBLIC;

	MSG_OUTLN("Connecting to: " << device);
	central->connect(device,
	// connect success callback
	[central, device, uuid, selectedOp, hexval]() {

		// One big connection success callback...
		MSG_OUTLN("Connection established");

		// do read/write depending on selected op
		if (selectedOp == READ_OP) {
			/* ************* READ CALL ************ */
			MSG_OUTLN("READ on " << uuid );
			// perform the op
			central->read(device, uuid,
					// on success
					[](const Gattlib::BinaryBuffer & data) {

						// got them tasty bytes back
						MSG_OUT("Bytes received: " );
						bool allAscii = true;
						for (Gattlib::BinaryBuffer::const_iterator iter=data.begin();
								iter != data.end();
								iter++) {
							MSG_OUT((int)*iter << " ");
							if ((*iter) < 0x09 || (*iter) > 0x7E) {
								allAscii = false;
							}
						}
						MSG_OUT(std::endl);

						// they were all 'printable' bytes, so lets do that too
						if (allAscii) {
							MSG_OUT("As ASCII: ");
							for (Gattlib::BinaryBuffer::const_iterator iter=data.begin();
									iter != data.end();
									iter++) {
								MSG_OUT((char)*iter);
							}
							MSG_OUT(std::endl);
						}

						doContinue = false;

					},
					// on fail:
					failedCallback
			);
		} else if (selectedOp == WRITE_OP) {

			/* ***************** Write call ******************* */
			long int tmpval = hexval;
			MSG_OUTLN("Write to " << uuid );
			Gattlib::BinaryBuffer buf;
			for (uint8_t i=0; i<sizeof(tmpval); i++) {
				buf.push_back(tmpval & 0xff);
				tmpval = tmpval >> 8;
			}
#ifdef DBG_ENABLED
			DBG_OUT("Will send (LSB):");
			for (uint8_t i=0; i<sizeof(hexval); i++) {
				DBG_OUT("0x" << std::hex << (int)buf[i] << " ");
			}
#endif
			// actually perform the op
			central->write(device, uuid, buf,
					// on success:
					[]() {
						MSG_OUTLN("Value written! Quitting now.");
						doContinue = false;
					},
					// on fail:
					failedCallback
			);

		} else {
			MSG_OUTLN("Unsupported operation? Quitting");
			doContinue = false;
		}

	});

}

