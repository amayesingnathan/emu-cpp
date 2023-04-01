module DebugState;

import Gameboy.Memory.AddressBus;

namespace GB {

	InternalState::InternalState() :
		LCDC(AddressBus::Read(Addr::LCDC)),
		STAT(AddressBus::Read(Addr::STAT)),
		SCY(AddressBus::Read(Addr::SCY)),
		SCX(AddressBus::Read(Addr::SCX)),
		LY(AddressBus::Read(Addr::LY)),
		LYC(AddressBus::Read(Addr::LYC)),

		DMA(AddressBus::Read(Addr::DMA)),
		BGP(AddressBus::Read(Addr::BGP)),
		OBP0(AddressBus::Read(Addr::OBP0)),
		OBP1(AddressBus::Read(Addr::OBP1)),
		WY(AddressBus::Read(Addr::WY)),
		WX(AddressBus::Read(Addr::WX)),

		JOYP(AddressBus::Read(Addr::JOYP)),
		SB(AddressBus::Read(Addr::SB)),
		SC(AddressBus::Read(Addr::SC)),

		DIV(AddressBus::Read(Addr::DIV)),
		TIMA(AddressBus::Read(Addr::TIMA)),
		TMA(AddressBus::Read(Addr::TMA)),
		TMC(AddressBus::Read(Addr::TMC)),

		IF(AddressBus::Read(Addr::IF)),
		IE(AddressBus::Read(Addr::IE))
	{}
}