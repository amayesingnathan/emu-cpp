#pragma once

/*
	
	Memory Bus Idea

	Memory blocks inherits from base class that registers it with the address bus. Memory block base class should have private read/write functions.
	Memory block gets used in class that owns it (e.g. WRAM in CPU) but will never get accessed by the class.
	Address bus then maps requested memory read and writes to the correct target memory block.
	Look into efficiencies in storing memory blocks contiguously.

	Address bus should store details of the registered memory block types (ROM/RAM).
	Store array of memory blocks structs (base class pointers + enum to decode it).
	Switch statement to dynamic cast back to specialised block.

	
	Memory Bank Controller (MBC)
	
	If the cartridge loaded supports MBC, the cartridge memory block could reroute read/writes to the MBC.
	Different MBC types (MBC1/2, possibly more later, if any to start), could inherit from MBC virtual class,
	which should keep track of which bank is loaded and interpret the reads and writes to the correct portion of the ROM.

	MBC should be part of the memory block derived class used in the cartridge.

*/