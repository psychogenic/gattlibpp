GattLib++
=========

Copyright (C) 2017 Pat Deegan, [psychogenic.com](https://psychogenic.com)

GattLib++ is a C++ wrapper around the ([async version of](https://github.com/psychogenic/gattlib "async version")) the [GattLib library](https://github.com/labapart/gattlib "GattLib library").

GattLib and GattLib++ allow access the to Generic Attribute Profile (GATT) protocol of BLE (Bluetooth Low Energy) devices, to 
	
	* scan and discover
	* connect/disconnect
	* read/write
	* register for notifications


If you want plain C, just use GattLib. It's sweet.  

I like C++.  In this case, one of the main advantages of GattLib++ is to leverage the power of lambdas to implement the callbacks used with the asynchronous function calls, e.g.

```
 // ...
 ble->connect("SO:ME:UU:ID",
	// connect success callback
	[]() {
		std::cout << "Yay, we're connected!" << std::endl;
	},
	// failure callback
	[]() {
		std::cout << "Boooo, failed to connect" << std::endl;
	});

```

This code has only been tested with Linux and Bluez on the D-Bus, but should work anywhere you can get `gattlib` working.  





Latest GattLib++ Release packages
=================================

* For x86_64, using GattLib ([async version](https://github.com/psychogenic/gattlib ):


    - ZIP: <https://github.com/psychogenic/gattlibpp/releases/download/v1.0.3/gattlibpp_1.0.3_.zip>
    - DEB: <https://github.com/psychogenic/gattlibpp/releases/download/v1.0.3/gattlibpp_1.0.3_.deb>
    - RPM: <https://github.com/psychogenic/gattlibpp/releases/download/v1.0.3/gattlibpp_1.0.3_.rpm>



Build GattLib++
===============

* Gattlib++ requires `gattlib` (the async version, available at on [my forked version](https://github.com/psychogenic/gattlib)), and that's it.

To build:

```
cd <gattlib++-src-root>
mkdir build && cd build
cmake ..
make
```


Examples
========


* [Coraline BLE plugin](https://github.com/psychogenic/coraline-plugin-ble-central)
A real-world demo using GattLib++ to provide BLE functionality to ionic/cordova apps running on Linux desktops (through [Coraline](https://coraline.psychogenic.com/))


* [Demonstrate scanning and discovery](/examples/ble_scan/ble_scan.cpp):

        ./examples/ble_scan/ble_scan (to scan for devices)
        ./examples/ble_scan/ble_scan 78:A5:04:22:45:4F (to scan and, if MAC present, connect+discover)
        

* [Read/Write demo](/examples/read_write/read_write.cpp):

        ./examples/read_write/read_write 78:A5:04:22:45:4F read 0x2a28 
        ./examples/read_write/read_write 78:A5:04:22:45:4F write 713d0002-503e-4c75-ba94-3148f18d941e 0x727170696867


Quick Start
===========

GattLib++ provides a BLECentral singleton, accessed through `getInstance()`:

```
  Gattlib::BLECentral* central = Gattlib::BLECentral::getInstance();
```

You basically set it up, make requests on that object as desired, and "tick" it periodically (in the main loop, in a thread, whatever you like).

With the central in hand: 

	1. Enable the device ``central->enable(successCb, failureCb)``
	2. Regularly "tick" the central, to give it time to process events: `central->processAsync()`
	3. Make `connect()`, `scan()`, `read()` and `write()` requests as required; and
	4. Leave the BLE adapter in a nice state, by calling `central->shutdown()` before you exit your program.



Known limitations
=================

	1. Gotta tick
	2. one-at-a-time a) devices
	3. one-at-a-time b) calls
	4. no advertising
	
### Ticking the central
First thing: if you don't **tick** the central, you won't get any results/callbacks triggered.  So call `processAsync()` early, call it often.


### One device
Though it may be relatively simple to implement dealing with multiple connected devices simultaneously, this currently isn't implemented.

### One operation
GattLib++ provides **async** operations but they aren't __queued__ or __threaded__ or anything.  So, while your program can continue doing it's business while e.g. we wait on a read, at this time you can only schedule one operation at a time.  If you have a string of things to do, say

	* connect
	* read some attribute
	* write to another

you'll either need to make a string of nested callbacks (so when connect says all is well, then start the read, and when you get the value, then start the write), or you'll need to get fancy and wrap things up in promise-like objects etc.  The point is: do **one** BLE operation after the other.

### No advertising
At this stage, gattlib doesn't seem to support reporting advertising data received during scans: all we get are device addresses and names.  This annoys me, and I'll be looking into it further.

License
=======

	GattLib++ Copyright (C) 2017 Pat Deegan, https://psychogenic.com
   
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

