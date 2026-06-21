// This file is included separately for each engine version

namespace GOTHIC_NAMESPACE
{
	// G1C: 0x005B5CB0 protected: enum zTResourceCacheState __thiscall zCResourceManager::CacheIn(class zCResource *,float)
	// G2A: 0x005DD040 protected: enum zTResourceCacheState __thiscall zCResourceManager::CacheIn(class zCResource *,float)
	zTResourceCacheState __fastcall zCResourceManager_CacheIn(zCResourceManager* self, void* vtable, zCResource* resource, float param_float);
	auto Hook_zCResourceManager_CacheIn = Union::CreateHook(reinterpret_cast<void*>(
		zSwitch(0x005B5CB0,0x005DD040)),
		&zCResourceManager_CacheIn, Union::HookType::Hook_Detours);
	zTResourceCacheState __fastcall zCResourceManager_CacheIn(zCResourceManager* self, void* vtable, zCResource* resource, float param_float)
	{
		zTResourceCacheState resource_cache_state = Hook_zCResourceManager_CacheIn(self, vtable, resource, param_float);
		hook_post_cache_in(resource);

		return resource_cache_state;
	}

	// G1C: 0x005B5FB0 protected: void __thiscall zCResourceManager::CacheOut(class zCResource *)
	// G2A: 0x005DD350 protected: void __thiscall zCResourceManager::CacheOut(class zCResource *)
	void __fastcall zCResourceManager_CacheOut(zCResourceManager* self, void* vtable, zCResource* resource);
	auto Hook_zCResourceManager_CacheOut = Union::CreateHook(reinterpret_cast<void*>(
		zSwitch(0x005B5FB0,0x005DD350)),
		&zCResourceManager_CacheOut, Union::HookType::Hook_Detours);
	void __fastcall zCResourceManager_CacheOut(zCResourceManager* self, void* vtable, zCResource* resource)
	{
		hook_pre_cache_out(resource);
		Hook_zCResourceManager_CacheOut(self, vtable, resource);
	}

	static auto partial_hook_menu_startup = Union::CreatePartialHook(reinterpret_cast<void*>
		(0x004DA343), []()
		{
			plugin_init();
			hook_after_game_init_and_before_menu_appear();
		}
	);

	// G1C: 0x004D6F30 public: virtual int __thiscall zCMenuItemChoice::ToggleValue(int,int)
	// G2A: 0x004E4080 public: virtual int __thiscall zCMenuItemChoice::ToggleValue(int,int)
	void __fastcall zCMenuItem_ToggleValue(zCMenuItemChoice* self, void* vtable, int v1, int v2);
	auto Hook_zCMenuItem_ToggleValue = Union::CreateHook(reinterpret_cast<void*>(
		zSwitch(0x004D6F30, 0x004E4080)),
		&zCMenuItem_ToggleValue, Union::HookType::Hook_Detours);
	void __fastcall zCMenuItem_ToggleValue(zCMenuItemChoice* self, void* vtable, int v1, int v2)
	{
		Hook_zCMenuItem_ToggleValue(self, vtable, v1, v2);
		menu_item_toggle_value(self, v1, v2);
	}

	void __fastcall zCMenu_Render(zCMenu* self, void* vtable);
	auto Hook_zCMenu_Render = Union::CreateHook(reinterpret_cast<void*>(
		zSwitch(0x004D0DA0, 0x004E14E0, 0x004DB270, 0x004DDC20)),
		&zCMenu_Render, Union::HookType::Hook_Detours);
	void __fastcall zCMenu_Render(zCMenu* self, void* vtable)
	{
		Hook_zCMenu_Render(self, vtable);
		menu_loop();
	}
}