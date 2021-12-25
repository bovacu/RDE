// Created by borja on 24/12/21.

#include "TextureRegion.h"

namespace engine {

    TextureRegion::TextureRegion(const TexturePtr& _texture, const Vec2F &_bottomLeft, const Vec2F &_topRight, const Vec2I& _size)
            : texture(_texture) {
        this->textureCoords[0] = { _bottomLeft.x, _bottomLeft.y };
        this->textureCoords[1] = { _topRight.x  , _bottomLeft.y };
        this->textureCoords[2] = { _topRight.x  , _topRight.y   };
        this->textureCoords[3] = { _bottomLeft.x, _topRight.y   };

        this->size = _size;
    }

    TextureRegionPtr TextureRegion::create(const TexturePtr& _texture, const Vec2F& _coordinates, const Vec2I & _regionSize, const Vec2I& _numberOfRegions) {
        Vec2F _bottomLeft = { (_coordinates.x * _regionSize.x) / _texture->getWidth(), (_coordinates.y * _regionSize.y) / _texture->getHeight() };
        Vec2F _topRight   = { ((_coordinates.x + _numberOfRegions.x) * _regionSize.x) / _texture->getWidth(),
                              ((_coordinates.y + _numberOfRegions.y) * _regionSize.y) / _texture->getHeight() };

        return std::make_shared<TextureRegion>(_texture, _bottomLeft, _topRight, _regionSize);
    }
}