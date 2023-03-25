export module EmuBase.Core.Common;

export import <concepts>;
export import <ranges>;
export import <cassert>;
export import <memory>;
export import <string_view>;

export namespace Emu {

	using uint = unsigned int;

	using usize = size_t;

	template<typename T>
	using Impl = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... TArgs>
	Impl<T> MakeInstance(TArgs&&... args) { return std::make_unique<T>(std::forward<TArgs>(args)...); }

	template<typename T, typename... TArgs>
	Ref<T> MakeRef(TArgs&&... args) { return std::make_shared<T>(std::forward<TArgs>(args)...); }


	template<bool Expr>
	consteval void SAssert(std::string_view msg = "")
	{
		static_assert(Expr);
	}

	inline void Assert(bool cond, std::string_view msg = "") { assert(cond); }

	inline auto Bind_Member_Func(auto* self, auto&& args)
	{
		return [self](auto&&... args) -> decltype(auto) { return self->func(std::forward<decltype(args)>(args)...); };
	}
}