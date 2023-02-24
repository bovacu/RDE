#include "core/DeviceInfoProvider.h"
#include "core/platform/PlatformHeaderSDL.h"

#include "SDL_power.h"

namespace RDE {

	void DeviceInfoProvider::init(Engine* _engine) {
		engine = _engine;
	}

	BatteryStatus DeviceInfoProvider::getBatteryStatus() {
		BatteryStatus _batteryStatus;
		int _secondsLeft = -1;
		int _percentage = -1;
		SDL_PowerState _powerState = SDL_POWERSTATE_UNKNOWN;
		
		#if IS_ANDROID()
		_powerState = SDL_GetPowerInfo(&_secondsLeft, &_percentage);
		_batteryStatus.platform = RDE_PLATFORM_TYPE_ANDROID;
		#endif
		
		#if IS_IOS()
		_powerState = SDL_GetPowerInfo(&_secondsLeft, &_percentage);
		_batteryStatus.platform = RDE_PLATFORM_TYPE_IOS;
		#endif
		
		#if IS_LINUX() && !IS_ANDROID()
		_powerState = SDL_GetPowerInfo(&_secondsLeft, &_percentage);
		_batteryStatus.platform = RDE_PLATFORM_TYPE_LINUX;
		#endif
		
		#if IS_WINDOWS() && !IS_ANDROID()
		_powerState = SDL_GetPowerInfo(&_secondsLeft, &_percentage);
		_batteryStatus.platform = RDE_PLATFORM_TYPE_WINDOWS;
		#endif
		
		#if IS_MAC() && !IS_ANDROID()
		_powerState = SDL_GetPowerInfo(&_secondsLeft, &_percentage);
		_batteryStatus.platform = RDE_PLATFORM_TYPE_MAC;
		#endif

		_batteryStatus.status = (BatteryStatus::RDE_BATTERY_)(int)_powerState;
		_batteryStatus.secondsLeft = _secondsLeft;
		_batteryStatus.batteryPercentage = _percentage >= 0 ? (float)_percentage / 100.f : -1.f;

		return _batteryStatus;
	}

}