#pragma once

namespace engine
{
    enum class Platform
    {
        mac,
        ios,
        android
    };

    Platform getCurrentPlatform();
} // namespace ast
