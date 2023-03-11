#ifndef RDE_DEVICE_INFO_PROVIDER_H
#define RDE_DEVICE_INFO_PROVIDER_H

#include "core/Enums.h"

namespace RDE {

	/**
	 * @brief Stores information about the battery levels.
	 */
	struct BatteryStatus {
		/**
			* @brief Seconds left. If -1 is returned is because value cannot be determined or 
			* because the device doesn't work on a battery.
			*/
		int secondsLeft = -1;

		/**
			* @brief Percentage of the battery [0,1]. If -1 is returned is because value cannot be determined or 
			* because the device doesn't work on a battery.
			*/
		float batteryPercentage = -1.f;

		RDE_BATTERY_ status = RDE_BATTERY_UKNOWN;
		RDE_PLATFORM_TYPE_ platform = RDE_PLATFORM_TYPE_UNSUPPORTED;
	};

	/**
	 * @brief Contains or gets information about certains parts of the device, such as the battery levels.
	*/
	class DeviceInfoProvider {
		public:
			/**
			 * @brief Returns the battery information on the platform the app is running on.
			 */
			BatteryStatus getBatteryStatus();
	};

}

#endif // RDE_DEVICE_INFO_PROVIDER_H