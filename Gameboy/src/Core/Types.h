#pragma once

#include "Base.h"

namespace GB {

    using Byte  = uint8_t;
    using SByte = int8_t;

    using Word  = uint16_t;
    using SWord  = int16_t;

    using USize = size_t;

    template<typename T>
    using Instance = std::unique_ptr<T>;

    template<typename T, typename... TArgs>
    Instance<T> MakeInstance(TArgs&&... args) { return make_unique<T>(std::forward<TArgs>(args)...); }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename... TArgs>
    Ref<T> MakeRef(TArgs&&... args) { return make_shared<T>(std::forward<TArgs>(args)...); }

	class BitField
	{
	public:
		BitField(Byte byte) : mByte(byte) {}

		bool bit(Byte bit) const { return mByte & GB_BIT(bit); }
		void set(Byte bit) { mByte |= GB_BIT(bit); }
		void reset(Byte bit) { mByte &= ~GB_BIT(bit); }

		operator Byte() const { return mByte; }

		BitField& operator&=(Byte flag)
		{
			mByte &= flag;
			return *this;
		}
		BitField& operator|=(Byte flag)
		{
			mByte |= flag;
			return *this;
		}

	private:
		Byte mByte;
	};
}