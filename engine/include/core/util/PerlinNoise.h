// Created by borja on 23/12/21.

#ifndef RESOURCES_GAME_PERLIN_NOISE_H
#define RESOURCES_GAME_PERLIN_NOISE_H

//----------------------------------------------------------------------------------------
//
//	siv::PerlinNoise
//	Perlin noise library for modern C++
//
//	Copyright (C) 2013-2021 Ryo Suzuki <reputeless@gmail.com>
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.
//
//----------------------------------------------------------------------------------------

# pragma once
# include <cstdint>
# include <algorithm>
# include <array>
# include <iterator>
# include <numeric>
# include <random>
# include <type_traits>

# if __has_include(<concepts>) && defined(__cpp_concepts)
#	include <concepts>
# endif


// Library major version
# define SIVPERLIN_VERSION_MAJOR			3

// Library minor version
# define SIVPERLIN_VERSION_MINOR			0

// Library revision version
# define SIVPERLIN_VERSION_REVISION			0

// Library version
# define SIVPERLIN_VERSION			((SIVPERLIN_VERSION_MAJOR * 100 * 100) + (SIVPERLIN_VERSION_MINOR * 100) + (SIVPERLIN_VERSION_REVISION))


// [[nodiscard]] for constructors
# if (201907L <= __has_cpp_attribute(nodiscard))
#	define SIVPERLIN_NODISCARD_CXX20 [[nodiscard]]
# else
#	define SIVPERLIN_NODISCARD_CXX20
# endif


// std::uniform_random_bit_generator concept
# if __cpp_lib_concepts
#	define SIVPERLIN_CONCEPT_URBG  template <std::uniform_random_bit_generator URBG>
#	define SIVPERLIN_CONCEPT_URBG_ template <std::uniform_random_bit_generator URBG>
# else
#	define SIVPERLIN_CONCEPT_URBG  template <class URBG, std::enable_if_t<std::conjunction_v<std::is_invocable<URBG&>, std::is_unsigned<std::invoke_result_t<URBG&>>>>* = nullptr>
#	define SIVPERLIN_CONCEPT_URBG_ template <class URBG, std::enable_if_t<std::conjunction_v<std::is_invocable<URBG&>, std::is_unsigned<std::invoke_result_t<URBG&>>>>*>
# endif


// arbitrary value for increasing entropy
# ifndef SIVPERLIN_DEFAULT_Y
#	define SIVPERLIN_DEFAULT_Y (0.12345)
# endif

// arbitrary value for increasing entropy
# ifndef SIVPERLIN_DEFAULT_Z
#	define SIVPERLIN_DEFAULT_Z (0.34567)
# endif


namespace siv {
    template <class Float>
    class BasicPerlinNoise
    {
        public:

        static_assert(std::is_floating_point_v<Float>);

        ///////////////////////////////////////
        //
        //	Typedefs
        //

        using state_type = std::array<std::uint8_t, 256>;

        using value_type = Float;

        using default_random_engine = std::mt19937;

        using seed_type = typename default_random_engine::result_type;

        ///////////////////////////////////////
        //
        //	Constructors
        //

        SIVPERLIN_NODISCARD_CXX20
        constexpr BasicPerlinNoise() noexcept;

        SIVPERLIN_NODISCARD_CXX20
        explicit BasicPerlinNoise(seed_type seed);

        SIVPERLIN_CONCEPT_URBG
        SIVPERLIN_NODISCARD_CXX20
        explicit BasicPerlinNoise(URBG&& urbg);

        ///////////////////////////////////////
        //
        //	Reseed
        //

        void reseed(seed_type seed);

        SIVPERLIN_CONCEPT_URBG
        void reseed(URBG&& urbg);

        ///////////////////////////////////////
        //
        //	Serialization
        //

        [[nodiscard]]
        constexpr const state_type& serialize() const noexcept;

        constexpr void deserialize(const state_type& state) noexcept;

        ///////////////////////////////////////
        //
        //	Noise (The result is in the range [-1, 1])
        //

        [[nodiscard]]
        value_type noise1D(value_type x) const noexcept;

        [[nodiscard]]
        value_type noise2D(value_type x, value_type y) const noexcept;

        [[nodiscard]]
        value_type noise3D(value_type x, value_type y, value_type z) const noexcept;

        ///////////////////////////////////////
        //
        //	Noise (The result is remapped to the range [0, 1])
        //

        [[nodiscard]]
        value_type noise1D_01(value_type x) const noexcept;

        [[nodiscard]]
        value_type noise2D_01(value_type x, value_type y) const noexcept;

        [[nodiscard]]
        value_type noise3D_01(value_type x, value_type y, value_type z) const noexcept;

        ///////////////////////////////////////
        //
        //	Octave noise (The result can be out of the range [-1, 1])
        //

        [[nodiscard]]
        value_type octave1D(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type octave2D(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type octave3D(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        ///////////////////////////////////////
        //
        //	Octave noise (The result is clamped to the range [-1, 1])
        //

        [[nodiscard]]
        value_type octave1D_11(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type octave2D_11(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type octave3D_11(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        ///////////////////////////////////////
        //
        //	Octave noise (The result is clamped and remapped to the range [0, 1])
        //

        [[nodiscard]]
        value_type octave1D_01(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type octave2D_01(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type octave3D_01(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        ///////////////////////////////////////
        //
        //	Octave noise (The result is normalized to the range [-1, 1])
        //

        [[nodiscard]]
        value_type normalizedOctave1D(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type normalizedOctave2D(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type normalizedOctave3D(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        ///////////////////////////////////////
        //
        //	Octave noise (The result is normalized and remapped to the range [0, 1])
        //

        [[nodiscard]]
        value_type normalizedOctave1D_01(value_type x, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type normalizedOctave2D_01(value_type x, value_type y, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        [[nodiscard]]
        value_type normalizedOctave3D_01(value_type x, value_type y, value_type z, std::int32_t octaves, value_type persistence = value_type(0.5)) const noexcept;

        private:

        state_type m_permutation;
    };

    using PerlinNoise = BasicPerlinNoise<double>;

    namespace perlin_detail
    {
        ////////////////////////////////////////////////
        //
        //	These functions are provided for consistency.
        //	You may get different results from std::shuffle() with different standard library implementations.
        //
        SIVPERLIN_CONCEPT_URBG
        [[nodiscard]]
        inline std::uint64_t Random(const std::uint64_t max, URBG&& urbg)
        {
            return (urbg() % (max + 1));
        }

        template <class RandomIt, class URBG>
        inline void Shuffle(RandomIt first, RandomIt last, URBG&& urbg)
        {
            if (first == last)
            {
                return;
            }

            using difference_type = typename std::iterator_traits<RandomIt>::difference_type;

            for (RandomIt it = first + 1; it < last; ++it)
            {
                const std::uint64_t n = static_cast<std::uint64_t>(it - first);
                std::iter_swap(it, first + static_cast<difference_type>(Random(n, std::forward<URBG>(urbg))));
            }
        }
        //
        ////////////////////////////////////////////////

        template <class Float>
        [[nodiscard]]
        inline constexpr Float Fade(const Float t) noexcept
        {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }

        template <class Float>
        [[nodiscard]]
        inline constexpr Float Lerp(const Float a, const Float b, const Float t) noexcept
        {
            return (a + (b - a) * t);
        }

        template <class Float>
        [[nodiscard]]
        inline constexpr Float Grad(const std::uint8_t hash, const Float x, const Float y, const Float z) noexcept
        {
            const std::uint8_t h = hash & 15;
            const Float u = h < 8 ? x : y;
            const Float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
            return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        }

        template <class Float>
        [[nodiscard]]
        inline constexpr Float Remap_01(const Float x) noexcept
        {
            return (x * Float(0.5) + Float(0.5));
        }

        template <class Float>
        [[nodiscard]]
        inline constexpr Float Clamp_11(const Float x) noexcept
        {
            return std::clamp(x, Float(-1.0), Float(1.0));
        }

        template <class Float>
        [[nodiscard]]
        inline constexpr Float RemapClamp_01(const Float x) noexcept
        {
            if (x <= Float(-1.0))
            {
                return Float(0.0);
            }
            else if (Float(1.0) <= x)
            {
                return Float(1.0);
            }

            return (x * Float(0.5) + Float(0.5));
        }

        template <class Noise, class Float>
        [[nodiscard]]
        inline auto Octave1D(const Noise& noise, Float x, const std::int32_t octaves, const Float persistence) noexcept
        {
            using value_type = Float;
            value_type result = 0;
            value_type amplitude = 1;

            for (std::int32_t i = 0; i < octaves; ++i)
            {
                result += (noise.noise1D(x) * amplitude);
                x *= 2;
                amplitude *= persistence;
            }

            return result;
        }

        template <class Noise, class Float>
        [[nodiscard]]
        inline auto Octave2D(const Noise& noise, Float x, Float y, const std::int32_t octaves, const Float persistence) noexcept
        {
            using value_type = Float;
            value_type result = 0;
            value_type amplitude = 1;

            for (std::int32_t i = 0; i < octaves; ++i)
            {
                result += (noise.noise2D(x, y) * amplitude);
                x *= 2;
                y *= 2;
                amplitude *= persistence;
            }

            return result;
        }

        template <class Noise, class Float>
        [[nodiscard]]
        inline auto Octave3D(const Noise& noise, Float x, Float y, Float z, const std::int32_t octaves, const Float persistence) noexcept
        {
            using value_type = Float;
            value_type result = 0;
            value_type amplitude = 1;

            for (std::int32_t i = 0; i < octaves; ++i)
            {
                result += (noise.noise3D(x, y, z) * amplitude);
                x *= 2;
                y *= 2;
                z *= 2;
                amplitude *= persistence;
            }

            return result;
        }

        template <class Float>
        [[nodiscard]]
        inline constexpr Float MaxAmplitude(const std::int32_t octaves, const Float persistence) noexcept
        {
            using value_type = Float;
            value_type result = 0;
            value_type amplitude = 1;

            for (std::int32_t i = 0; i < octaves; ++i)
            {
                result += amplitude;
                amplitude *= persistence;
            }

            return result;
        }
    }

    ///////////////////////////////////////

    template <class Float>
    inline constexpr BasicPerlinNoise<Float>::BasicPerlinNoise() noexcept
            : m_permutation{ 151,160,137,91,90,15,
                             131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
                             190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
                             88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
                             77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
                             102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
                             135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
                             5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
                             223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
                             129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
                             251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
                             49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
                             138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 } {}

    template <class Float>
    inline BasicPerlinNoise<Float>::BasicPerlinNoise(const seed_type seed)
    {
        reseed(seed);
    }

    template <class Float>
    SIVPERLIN_CONCEPT_URBG_
    inline BasicPerlinNoise<Float>::BasicPerlinNoise(URBG&& urbg)
    {
        reseed(std::forward<URBG>(urbg));
    }

    ///////////////////////////////////////

    template <class Float>
    inline void BasicPerlinNoise<Float>::reseed(const seed_type seed)
    {
        reseed(default_random_engine{ seed });
    }

    template <class Float>
    SIVPERLIN_CONCEPT_URBG_
    inline void BasicPerlinNoise<Float>::reseed(URBG&& urbg)
    {
        std::iota(m_permutation.begin(), m_permutation.end(), uint8_t{ 0 });

        perlin_detail::Shuffle(m_permutation.begin(), m_permutation.end(), std::forward<URBG>(urbg));
    }

    ///////////////////////////////////////

    template <class Float>
    inline constexpr const typename BasicPerlinNoise<Float>::state_type& BasicPerlinNoise<Float>::serialize() const noexcept
    {
        return m_permutation;
    }

    template <class Float>
    inline constexpr void BasicPerlinNoise<Float>::deserialize(const state_type& state) noexcept
    {
        m_permutation = state;
    }

    ///////////////////////////////////////

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise1D(const value_type x) const noexcept
    {
        return noise3D(x,
                       static_cast<value_type>(SIVPERLIN_DEFAULT_Y),
                       static_cast<value_type>(SIVPERLIN_DEFAULT_Z));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise2D(const value_type x, const value_type y) const noexcept
    {
        return noise3D(x,
                       y,
                       static_cast<value_type>(SIVPERLIN_DEFAULT_Z));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise3D(const value_type x, const value_type y, const value_type z) const noexcept
    {
        const value_type _x = std::floor(x);
        const value_type _y = std::floor(y);
        const value_type _z = std::floor(z);

        const std::int32_t ix = static_cast<std::int32_t>(_x) & 255;
        const std::int32_t iy = static_cast<std::int32_t>(_y) & 255;
        const std::int32_t iz = static_cast<std::int32_t>(_z) & 255;

        const value_type fx = (x - _x);
        const value_type fy = (y - _y);
        const value_type fz = (z - _z);

        const value_type u = perlin_detail::Fade(fx);
        const value_type v = perlin_detail::Fade(fy);
        const value_type w = perlin_detail::Fade(fz);

        const std::uint8_t A = (m_permutation[ix & 255] + iy) & 255;
        const std::uint8_t B = (m_permutation[(ix + 1) & 255] + iy) & 255;

        const std::uint8_t AA = (m_permutation[A] + iz) & 255;
        const std::uint8_t AB = (m_permutation[(A + 1) & 255] + iz) & 255;

        const std::uint8_t BA = (m_permutation[B] + iz) & 255;
        const std::uint8_t BB = (m_permutation[(B + 1) & 255] + iz) & 255;

        const value_type p0 = perlin_detail::Grad(m_permutation[AA], fx, fy, fz);
        const value_type p1 = perlin_detail::Grad(m_permutation[BA], fx - 1, fy, fz);
        const value_type p2 = perlin_detail::Grad(m_permutation[AB], fx, fy - 1, fz);
        const value_type p3 = perlin_detail::Grad(m_permutation[BB], fx - 1, fy - 1, fz);
        const value_type p4 = perlin_detail::Grad(m_permutation[(AA + 1) & 255], fx, fy, fz - 1);
        const value_type p5 = perlin_detail::Grad(m_permutation[(BA + 1) & 255], fx - 1, fy, fz - 1);
        const value_type p6 = perlin_detail::Grad(m_permutation[(AB + 1) & 255], fx, fy - 1, fz - 1);
        const value_type p7 = perlin_detail::Grad(m_permutation[(BB + 1) & 255], fx - 1, fy - 1, fz - 1);

        const value_type q0 = perlin_detail::Lerp(p0, p1, u);
        const value_type q1 = perlin_detail::Lerp(p2, p3, u);
        const value_type q2 = perlin_detail::Lerp(p4, p5, u);
        const value_type q3 = perlin_detail::Lerp(p6, p7, u);

        const value_type r0 = perlin_detail::Lerp(q0, q1, v);
        const value_type r1 = perlin_detail::Lerp(q2, q3, v);

        return perlin_detail::Lerp(r0, r1, w);
    }

    ///////////////////////////////////////

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise1D_01(const value_type x) const noexcept
    {
        return perlin_detail::Remap_01(noise1D(x));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise2D_01(const value_type x, const value_type y) const noexcept
    {
        return perlin_detail::Remap_01(noise2D(x, y));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::noise3D_01(const value_type x, const value_type y, const value_type z) const noexcept
    {
        return perlin_detail::Remap_01(noise3D(x, y, z));
    }

    ///////////////////////////////////////

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave1D(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Octave1D(*this, x, octaves, persistence);
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave2D(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Octave2D(*this, x, y, octaves, persistence);
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave3D(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Octave3D(*this, x, y, z, octaves, persistence);
    }

    ///////////////////////////////////////

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave1D_11(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Clamp_11(octave1D(x, octaves, persistence));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave2D_11(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Clamp_11(octave2D(x, y, octaves, persistence));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave3D_11(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Clamp_11(octave3D(x, y, z, octaves, persistence));
    }

    ///////////////////////////////////////

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave1D_01(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::RemapClamp_01(octave1D(x, octaves, persistence));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave2D_01(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::RemapClamp_01(octave2D(x, y, octaves, persistence));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::octave3D_01(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::RemapClamp_01(octave3D(x, y, z, octaves, persistence));
    }

    ///////////////////////////////////////

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave1D(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return (octave1D(x, octaves, persistence) / perlin_detail::MaxAmplitude(octaves, persistence));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave2D(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return (octave2D(x, y, octaves, persistence) / perlin_detail::MaxAmplitude(octaves, persistence));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave3D(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return (octave3D(x, y, z, octaves, persistence) / perlin_detail::MaxAmplitude(octaves, persistence));
    }

    ///////////////////////////////////////

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave1D_01(const value_type x, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Remap_01(normalizedOctave1D(x, octaves, persistence));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave2D_01(const value_type x, const value_type y, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Remap_01(normalizedOctave2D(x, y, octaves, persistence));
    }

    template <class Float>
    inline typename BasicPerlinNoise<Float>::value_type BasicPerlinNoise<Float>::normalizedOctave3D_01(const value_type x, const value_type y, const value_type z, const std::int32_t octaves, const value_type persistence) const noexcept
    {
        return perlin_detail::Remap_01(normalizedOctave3D(x, y, z, octaves, persistence));
    }
}

typedef uint32_t PerlinSeed;
struct NoiseMap {
    float** map;
    int width, height, octaves;
    float frequency;
    PerlinSeed seed;
};
class PerlinNoiseManager {
    private:
        std::unordered_map<PerlinSeed, NoiseMap> maps;
        int counter;
        siv::PerlinNoise perlin{ 0 };
    public:
        NoiseMap& generate2DMap(int _width, int _height, float _frequency, int _octaves, PerlinSeed _seed) {
            perlin.reseed(_seed);
            NoiseMap _noiseMap {nullptr, _width, _height, _octaves, _frequency, _seed};

            double _fx = (_frequency / (float)_width);
            double _fy = (_frequency / (float)_height);

            auto _map = new float*[_height];

            for (auto _y = 0; _y < _height; ++_y) {
                _map[_y] = new float[_width];
                for (auto _x = 0; _x < _width; ++_x) {
                    _map[_y][_x] = (float)perlin.octave2D_11((_x * _fx), (_y * _fy), _octaves);
                }
            }

            _noiseMap.map = _map;
            maps[_seed] = _noiseMap;

            return maps[_seed];
        }

        float getPerlinNoise2DPointValue(int _x, int _y, int _width, int _height, float _frequency, int _octaves, PerlinSeed _seed) {
            perlin.reseed(_seed);
            double _fx = (_frequency / (float)_width);
            double _fy = (_frequency / (float)_height);
            return (float)perlin.octave2D_11((_x * _fx), (_y * _fy), _octaves);
        }

        void clean() {
            for(auto& _map : maps) {
                for (auto _y = 0; _y < _map.second.height; _y++)
                    delete[] _map.second.map[_y];
                delete [] _map.second.map;
            }
        }

        NoiseMap& getNoiseMap(PerlinSeed _seed) {
            return maps[_seed];
        }

        void removeNoiseMap(PerlinSeed _seed) {
            auto* _map = &maps[_seed];
            for (auto _y = 0; _y < _map->height; _y++)
                delete[] _map->map[_y];
            delete [] _map->map;

            maps.erase(_seed);
        }
};

# undef SIVPERLIN_NODISCARD_CXX20
# undef SIVPERLIN_CONCEPT_URBG
# undef SIVPERLIN_CONCEPT_URBG_

#endif //RESOURCES_GAME_PERLIN_NOISE_H


// -------------------------> EXAMPLE
//# include <cassert>
//# include <iostream>
//# include <fstream>
//# include <sstream>
//# include "core/util/PerlinNoise.h"
//
//# pragma pack (push, 1)
//struct BMPHeader
//{
//    std::uint16_t bfType;
//    std::uint32_t bfSize;
//    std::uint16_t bfReserved1;
//    std::uint16_t bfReserved2;
//    std::uint32_t bfOffBits;
//    std::uint32_t biSize;
//    std::int32_t  biWidth;
//    std::int32_t  biHeight;
//    std::uint16_t biPlanes;
//    std::uint16_t biBitCount;
//    std::uint32_t biCompression;
//    std::uint32_t biSizeImage;
//    std::int32_t  biXPelsPerMeter;
//    std::int32_t  biYPelsPerMeter;
//    std::uint32_t biClrUsed;
//    std::uint32_t biClrImportant;
//};
//static_assert(sizeof(BMPHeader) == 54);
//# pragma pack (pop)
//
//struct RGB
//{
//    double r = 0.0;
//    double g = 0.0;
//    double b = 0.0;
//    constexpr RGB() = default;
//    explicit constexpr RGB(double _rgb) noexcept
//            : r{ _rgb }, g{ _rgb }, b{ _rgb } {}
//    constexpr RGB(double _r, double _g, double _b) noexcept
//            : r{ _r }, g{ _g }, b{ _b } {}
//};
//
//class Image
//{
//    public:
//
//    Image() = default;
//
//    Image(std::size_t width, std::size_t height)
//            : m_data(width* height)
//            , m_width{ static_cast<std::int32_t>(width) }
//            , m_height{ static_cast<std::int32_t>(height) } {}
//
//    void set(std::int32_t x, std::int32_t y, const RGB& color)
//    {
//        if (not inBounds(y, x))
//        {
//            return;
//        }
//
//        m_data[static_cast<std::size_t>(y) * m_width + x] = color;
//    }
//
//    std::int32_t width() const noexcept { return m_width; }
//
//    std::int32_t height() const noexcept { return m_height; }
//
//    bool saveBMP(const std::string& path)
//    {
//        const std::int32_t  rowSize = m_width * 3 + m_width % 4;
//        const std::uint32_t bmpsize = rowSize * m_height;
//        const BMPHeader header =
//                {
//                        0x4d42,
//                        static_cast<std::uint32_t>(bmpsize + sizeof(BMPHeader)),
//                        0, 0, sizeof(BMPHeader), 40,
//                        m_width, m_height, 1, 24,
//                        0, bmpsize, 0, 0, 0, 0
//                };
//
//        if (std::ofstream ofs{ path, std::ios_base::binary })
//        {
//            ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
//
//            std::vector<std::uint8_t> line(rowSize);
//
//            for (std::int32_t y = m_height - 1; -1 < y; --y)
//            {
//                size_t pos = 0;
//
//                for (std::int32_t x = 0; x < m_width; ++x)
//                {
//                    const RGB& col = m_data[static_cast<std::size_t>(y) * m_width + x];
//                    line[pos++] = ToUint8(col.b);
//                    line[pos++] = ToUint8(col.g);
//                    line[pos++] = ToUint8(col.r);
//                }
//
//                ofs.write(reinterpret_cast<const char*>(line.data()), line.size());
//            }
//
//            return true;
//        }
//        else
//        {
//            return false;
//        }
//    }
//
//    private:
//
//    std::vector<RGB> m_data;
//
//    std::int32_t m_width = 0, m_height = 0;
//
//    bool inBounds(std::int32_t y, std::int32_t x) const noexcept
//    {
//        return (0 <= y) && (y < m_height) && (0 <= x) && (x < m_width);
//    }
//
//    static constexpr std::uint8_t ToUint8(double x) noexcept
//    {
//        return (x <= 0.0) ? 0 : (1.0 <= x) ? 255 : static_cast<std::uint8_t>(x * 255.0 + 0.5);
//    }
//};
//
//void Test()
//{
//    siv::PerlinNoise perlinA{ std::random_device{} };
//    siv::PerlinNoise perlinB;
//
//    perlinB.deserialize(perlinA.serialize());
//
//    assert(perlinA.octave3D(0.1, 0.2, 0.3, 4)
//           == perlinB.octave3D(0.1, 0.2, 0.3, 4));
//
//    perlinA.reseed(12345u);
//    perlinB.reseed(12345u);
//
//    assert(perlinA.octave3D(0.1, 0.2, 0.3, 4)
//           == perlinB.octave3D(0.1, 0.2, 0.3, 4));
//
//    perlinA.reseed(std::mt19937{ 67890u });
//    perlinB.reseed(std::mt19937{ 67890u });
//
//    assert(perlinA.octave3D(0.1, 0.2, 0.3, 4)
//           == perlinB.octave3D(0.1, 0.2, 0.3, 4));
//
//    for (std::int32_t y = 0; y < 20; ++y)
//    {
//        for (std::int32_t x = 0; x < 20; ++x)
//        {
//            const double noise = perlinA.octave2D_01(x * 0.1, y * 0.1, 6);
//            std::cout << static_cast<int>(std::floor(noise * 10) - 0.5);
//        }
//        std::cout << '\n';
//    }
//}
//
//int main()
//{
//    Test();
//
//    Image image{ 512, 512 };
//
//    std::cout << "---------------------------------\n";
//    std::cout << "* frequency [0.1 .. 8.0 .. 64.0] \n";
//    std::cout << "* octaves   [1 .. 8 .. 16]       \n";
//    std::cout << "* seed      [0 .. 2^32-1]        \n";
//    std::cout << "---------------------------------\n";
//
//    for (;;)
//    {
//        double frequency;
//        std::cout << "double frequency = ";
//        std::cin >> frequency;
//        frequency = std::clamp(frequency, 0.1, 64.0);
//
//        std::int32_t octaves;
//        std::cout << "int32 octaves    = ";
//        std::cin >> octaves;
//        octaves = std::clamp(octaves, 1, 16);
//
//        std::uint32_t seed;
//        std::cout << "uint32 seed      = ";
//        std::cin >> seed;
//
//        const siv::PerlinNoise perlin{ seed };
//        const double fx = (frequency / image.width());
//        const double fy = (frequency / image.height());
//
//        for (std::int32_t y = 0; y < image.height(); ++y)
//        {
//            for (std::int32_t x = 0; x < image.width(); ++x)
//            {
//                const RGB color(perlin.octave2D_01((x * fx), (y * fy), octaves));
//                image.set(x, y, color);
//            }
//        }
//
//        std::stringstream ss;
//        ss << 'f' << frequency << 'o' << octaves << '_' << seed << ".bmp";
//
//        if (image.saveBMP(ss.str()))
//        {
//            std::cout << "...saved \"" << ss.str() << "\"\n";
//        }
//        else
//        {
//            std::cout << "...failed\n";
//        }
//
//        char c;
//        std::cout << "continue? [y/n] >";
//        std::cin >> c;
//        if (c != 'y') break;
//        std::cout << '\n';
//    }
//}
