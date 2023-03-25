export module Gameboy.Graphics.Sprite;

import Gameboy.Core.Types;

export namespace GB {

	struct Sprite
	{
		Byte xPos;
		Byte yPos;
		Byte tileLoc;
		ByteBits flags;
	};
}