// Created by borja on 14/2/22.

#include "core/procedural/CellularAutomataMapGenerator.h"
#include "core/render/elements/Texture.h"

namespace RDE {

    void CellularAutomata::init(const GenerationSettings& _settings) {
        map.settings = _settings;
        random.seedRandom(_settings.seed);
        create();
        fillData();
    }

    ProceduralMap& CellularAutomata::getMap() {
        return map;
    }

    void CellularAutomata::reset(const GenerationSettings& _settings) {
        destroy();
        map.settings = _settings;
        create();
        fillData();
    }

    CellularAutomata::~CellularAutomata() {
        destroy();
    }

    void CellularAutomata::create() {
        map.data = new float*[map.settings.size.y];
        for(int _y = 0; _y < map.settings.size.y; _y++)
            map.data[_y] = new float[map.settings.size.x];
    }

    void CellularAutomata::fillData() {
        CPUTexture _image;
        _image.init(map.settings.size.x, map.settings.size.y, ImageType::PNG);
        for(int _y = 0; _y < map.settings.size.y; _y++)
            for(int _x = 0; _x < map.settings.size.x; _x++)
                map.data[_y][_x] = random.randomf(0.0f, 1.f) > map.settings.fillAmount ? 0 : 1;

        for(int _i = 0; _i < map.settings.steps; _i++)
            step();

        for(int _y = 0; _y < map.settings.size.y; _y++)
            for(int _x = 0; _x < map.settings.size.x; _x++)
                _image.setPixel(_x, _y, map.data[_y][_x] == 1 ? Color::White : Color::Black);

        _image.saveAs("assets/cellular.png");
    }

    void CellularAutomata::destroy() const {
        for(int _y = 0; _y < map.settings.size.y; _y++)
            delete map.data[_y];

        delete [] map.data;
    }

    int CellularAutomata::getNeighbourCellCount(int _x, int _y) const {
        int neighbourCellCount = 0;
        if (_x > 0) {
            neighbourCellCount += (int)map.data[_x - 1][_y];
            if (_y > 0) {
                neighbourCellCount += (int)map.data[_x - 1][_y - 1];
            }
        }

        if (_y > 0) {
            neighbourCellCount += (int)map.data[_x][_y - 1];
            if (_x < map.settings.size.x - 1) {
                neighbourCellCount += (int)map.data[_x + 1][_y - 1];
            }
        }

        if (_x < map.settings.size.x - 1) {
            neighbourCellCount += (int)map.data[_x + 1][_y];
            if (_y < map.settings.size.y - 1) {
                neighbourCellCount += (int)map.data[_x + 1][_y + 1];
            }
        }

        if (_y < map.settings.size.y - 1) {
            neighbourCellCount += (int)map.data[_x][ _y + 1];
            if (_x > 0) {
                neighbourCellCount += (int)map.data[_x - 1][ _y + 1];
            }
        }

        return neighbourCellCount;
    }

    void CellularAutomata::step() {
       int _buffer[map.settings.size.x][map.settings.size.y];

        for (int x = 0; x < map.settings.size.x; ++x) {
            for (int y = 0; y < map.settings.size.y; ++y) {
                int liveCellCount = (int)map.data[x][y] + getNeighbourCellCount(x, y);
                _buffer[x][y] = liveCellCount > map.settings.numberOfNeighbours ? 1 : 0;
            }
        }

        for (int x = 0; x < map.settings.size.x; ++x) {
            for (int y = 0; y < map.settings.size.y; ++y) {
                map.data[x][y] = (float)_buffer[x][y];
            }
        }
    }


}
