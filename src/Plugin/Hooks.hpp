// This file is included separately for each engine version

namespace GOTHIC_NAMESPACE
{
	// // G1C: 0x004CEC30 public: virtual int __thiscall zCMenu::Run(void)
	// // G2A: 0x004DB9A0 public: virtual int __thiscall zCMenu::Run(void)
	// void __fastcall zCMenu_Run(zCMenu* self, void* vtable);
	// auto Hook_zCMenu_Run = Union::CreateHook(reinterpret_cast<void*>(
	// 	zSwitch(0x004CEC30, 0x004DB9A0)),
	// 	&zCMenu_Run, Union::HookType::Hook_Detours);
	// void __fastcall zCMenu_Run(zCMenu* self, void* vtable)
	// {
	// 	Hook_zCMenu_Run(self, vtable);
	// 	menu_run_hook(self);
	// }

	void __fastcall zCMenu_Startup(zCMenu* self, void* vtable);
	auto Hook_zCMenu_Startup = Union::CreateHook(reinterpret_cast<void*>(
		zSwitch(0x004CD3B0, 0x004D9F90)),
		&zCMenu_Startup, Union::HookType::Hook_Detours);
	void __fastcall zCMenu_Startup(zCMenu* self, void* vtable)
	{
		Hook_zCMenu_Startup(self, vtable);
		hook_after_game_init_and_before_menu_appear();
	}

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

	void __fastcall oCGame_Init(oCGame* self, void* vtable);
	auto Hook_oCGame_Init = Union::CreateHook(reinterpret_cast<void*>(
		zSwitch(0x00636F50, 0x0065D480, 0x006646D0, 0x006C1060)),
		&oCGame_Init, Union::HookType::Hook_Detours);
	void __fastcall oCGame_Init(oCGame* self, void* vtable)
	{
		Hook_oCGame_Init(self, vtable);
		hook_game_init();
	}
}