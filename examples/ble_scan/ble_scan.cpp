/*
 * ble_scan.cpp
 *
 *  Created on: Dec 18, 2017
 *
 *  Copyright (C) 2017 Pat Deegan, https://psychogenic.com
 *
 *  Usage:
 *    ble_scan <device_address_to_discover>
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


#include <iostream>
#include <Gattlibpp.h>
#include <unistd.h> // for usleep



#define BLE_SCAN_MAXTIME_SECONDS		10

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



bool doContinue = true;
int failCount = 0;
bool amConnected = false;

// a few forward decl
void useEnabledBLECentral (const std::string & targetDevice);
void failedCallback();

void showUsage() {
	std::cout << "gattlib++ ble scan. Usage:" << std::endl
				<< "ble_scan <device mac>" << std::endl;
}

int main(int argc, const char *argv[]) {

	std::string target;
	if (argc > 1) {
		target = argv[1];
	}

	// check if just a cry for help
	if (target == "-h" || target == "--help") {
		showUsage();
		return 0;
	}

	Gattlib::BLECentral::getInstance()->enable(
	// enable worked out:
	[target](){
		MSG_OUTLN("Device enabled");
		// ready to go!
		useEnabledBLECentral(target);
	},
	// enable failed?
	[]() {
		MSG_OUTLN("Could not even enable device, such sadness.");
		MSG_OUTLN("Check that bluetooth is up & running and that we can actually use it.");
		doContinue = false;

	}
	);


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

/*
 * We can have callbacks setup independently, like these two,
 * to avoid over complicated or repetitive nesting, or just use
 * inline lambdas, like below.
 */

void failedCallback() {

	if (! amConnected) {
		MSG_OUTLN("Last op failed, prior to even connecting... giving up.");
		doContinue = false;
		return;

	}

	if (failCount++ > 5) {
		MSG_OUTLN("Last op failed... one too many! aborting program.");
		doContinue = false;
	} else {

		DBG_OUTLN("Last op failed... hm.");
	}
}


void onConnect(const Gattlib::Discovery::Device & theDevice) {

	MSG_OUTLN("Connected to " << theDevice.id << ", yay!");
	amConnected = true;
	Gattlib::Characteristic::List chars =
			Gattlib::BLECentral::getInstance()->characteristicsFor(
					theDevice.id);

	DBG_OUTLN("Have found " << chars.size() << " characteristics.");

	for (Gattlib::Characteristic::List::iterator cIter = chars.begin();
			cIter != chars.end(); cIter++) {
		MSG_OUT("Characteristic '" << (*cIter).id << "' can be");

		uint8_t numOfInterest = 0;
		if ((*cIter).supportsReads()) {
			MSG_OUT(" read,");
			numOfInterest++;
		}

		if ((*cIter).supportsWrites()) {
			DBG_OUT(" written,");
			numOfInterest++;
		}

		if ((*cIter).supportSubscriptions()) {
			MSG_OUT(" subscribed to,");

			numOfInterest++;

		}

		if (numOfInterest) {
			MSG_OUT(" which is ");
			if (numOfInterest > 2) {
				MSG_OUT("really ");
			} else if (numOfInterest > 1) {
				MSG_OUT("pretty ");
			}
			MSG_OUT("cool." << std::endl);
		} else {

			MSG_OUT(" neither read/written nor subscribed to. hm" << std::endl);
		}

	}

	MSG_OUTLN("ok, we're done. seacrest, out.");
	doContinue = false;

}

void useEnabledBLECentral (const std::string & targetDevice)
{
	// ok, the ble central is up and running, lets do some stuff...
	Gattlib::BLECentral * central = Gattlib::BLECentral::getInstance();
	// I want to have access to characteristics as soon as we connect, so:
	central->setAutoDiscoverOnConnect(true);

	DBG_OUTLN("Bluetooth enabled.  Will scan for up to " << BLE_SCAN_MAXTIME_SECONDS << " seconds.");

	// now start the scan.
	// scan(TIMEOUT, DONECALLBACK, DISCOVEREDCALLBACK, FAILEDCB)
	central->scan(BLE_SCAN_MAXTIME_SECONDS,
			// done callback
			[central]() {
				// Done callback, meaning the timeout timed out.
				MSG_OUTLN("scan done!");
				// actually tell the lower layers to disable scanning:
				central->stopScan([]() {
							DBG_OUTLN("scan stopped!");
							doContinue = false;
						}, []() {
							DBG_OUTLN("scan stop failed -- I think we were already stopped there, son.");
						});

			},
			// discovered device callback
			[central, targetDevice](const Gattlib::Discovery::Device & foundDev) {
				// Discovered callback

				MSG_OUTLN("***** Found device "
						<< foundDev.id << " with name '"
						<< foundDev.name << "'");

				if (foundDev.id == targetDevice) {
					MSG_OUTLN(" Aborting scan preemptively, as we found our friend:"
							<< targetDevice << "...");

					central->stopScan([central, foundDev]() {

								DBG_OUTLN("scan stopped by our abort, attempting a connection!");
								central->connect(foundDev.id, [foundDev]() {
											// call the onConnect callback
											onConnect(foundDev);
										},failedCallback);
							}, []() {
								DBG_OUTLN("scan stop failed?");
							});
				}

			}, failedCallback);

}

