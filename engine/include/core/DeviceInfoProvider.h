#ifndef RDE_DEVICE_INFO_PROVIDER_H
#define RDE_DEVICE_INFO_PROVIDER_H

#include "core/platform/Platform.h"

namespace RDE {

	/**
	 * @brief Stores information about the battery levels.
	 */
	struct BatteryStatus {

		/**
		* @brief The status of the battery.
		*/
		enum RDE_BATTERY_ {
			RDE_BATTERY_UKNOWN,
			RDE_BATTERY_ON_BATTERY,
			RDE_BATTERY_NO_BATTERY,
			RDE_BATTERY_CHARGING,
			RDE_BATTERY_CHARGED
		};

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
		private:
			Engine* engine;

		public:
			/**
			 * @brief Initializes the DeviceInfoProvider to work correctly.
			 */
			void init(Engine* _engine);

			/**
			 * @brief Returns the battery information on the platform the app is running on.
			 */
			BatteryStatus getBatteryStatus();
	};

}

#endif // RDE_DEVICE_INFO_PROVIDER_H