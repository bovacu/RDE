// Created by borja on 14/2/22.


#ifndef ENGINE2_0_CELLULAR_AUTOMATA_MAP_GENERATOR_H
#define ENGINE2_0_CELLULAR_AUTOMATA_MAP_GENERATOR_H

#include "core/util/Util.h"

namespace GDE {

    struct GenerationSettings {
        Vec2I size = {128, 128};
        float fillAmount = 0.5f;
        int numberOfNeighbours = 4;
        int steps = 30;
        long seed = -1;
    };

    typedef float** Matrix2D;

    struct ProceduralMap {
        Matrix2D data;
        GenerationSettings settings;
    };

    class MapGenerator {
        public:
            virtual void init(const GenerationSettings& _settings) = 0;
            virtual ProceduralMap& getMap() = 0;
            virtual void reset(const GenerationSettings& _settings) = 0;
            virtual ~MapGenerator() = default;
    };

    class CellularAutomata : public MapGenerator {
        private:
            ProceduralMap map;
            Random random;

        private:
            void create();
            void destroy() const;
            void fillData();
            [[nodiscard]] int getNeighbourCellCount(int _x, int _y) const;
            void step();

        public:
            void init(const GenerationSettings& _settings) override;
            ProceduralMap& getMap() override;
            void reset(const GenerationSettings& _settings) override;
            ~CellularAutomata() override;
    };



}

#endif //ENGINE2_0_CELLULAR_AUTOMATA_MAP_GENERATOR_H
