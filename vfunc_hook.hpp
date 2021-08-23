#pragma once
#define NOMINMAX
#include <assert.h>
#include <Windows.h>

#include <cstdint>
#include <stdexcept>

namespace SDK
{


	namespace index
	{
		constexpr auto EmitSound1 = 5;
		constexpr auto EmitSound2 = 6;
		constexpr auto EndScene = 42;
		constexpr auto Reset = 16;
		constexpr auto PaintTraverse = 41;
		constexpr auto hkCreateMove = 22;
		constexpr auto CreateMove = 24;
		constexpr auto SoundPlay = 82;
		constexpr auto FrameStageNotify = 37;
		constexpr auto DrawModelExecute = 21;
		constexpr auto DoPostScreenSpaceEffects = 44;
		constexpr auto SvCheatsGetBool = 13;
		constexpr auto OverrideView = 18;
		constexpr auto LockCursor = 67;
		constexpr auto CamToFirstPerson = 36;
	}


	namespace detail
	{
		class protect_guard
		{
		public:
			protect_guard(void* base, size_t len, std::uint32_t flags)
			{
				_base = base;
				_length = len;
				if (!VirtualProtect(base, len, flags, (PDWORD)&_old))
					throw std::runtime_error("Failed to protect region.");
			}
			~protect_guard()
			{
				VirtualProtect(_base, _length, _old, (PDWORD)&_old);
			}

		private:
			void*         _base;
			size_t        _length;
			std::uint32_t _old;
		};
	}

	class vfunc_hook
	{
	public:
		vfunc_hook();
		vfunc_hook(void* base);
		~vfunc_hook();

		bool setup(void* class_base = nullptr);

		template<typename T>
		void hook_index(int index, T fun)
		{
			assert(index >= 0 && index <= (int)vftbl_len);
			new_vftbl[index + 1] = reinterpret_cast<std::uintptr_t>(fun);
		}
		void unhook_index(int index)
		{
			new_vftbl[index] = old_vftbl[index];
		}
		void unhook_all()
		{
			try {
				if (old_vftbl != nullptr) {
					auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
					*(std::uintptr_t**)class_base = old_vftbl;
					old_vftbl = nullptr;
				}
			}
			catch (...) {
			}
		}

		template<typename T>
		T get_original(int index)
		{
			return (T)old_vftbl[index];
		}

	private:
		static inline std::size_t estimate_vftbl_length(std::uintptr_t* vftbl_start);

		void*           class_base;
		std::size_t     vftbl_len;
		std::uintptr_t* new_vftbl;
		std::uintptr_t* old_vftbl;
	};
}
