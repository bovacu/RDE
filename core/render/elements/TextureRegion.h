// Created by borja on 24/12/21.


#ifndef ENGINE2_0_TEXTURE_REGION_H
#define ENGINE2_0_TEXTURE_REGION_H


#include "core/util/Util.h"
#include "Texture.h"

namespace engine {

    class TextureRegion;
    typedef std::shared_ptr<TextureRegion> TextureRegionPtr;

    class TextureRegion {
        private:
            std::shared_ptr<Texture> texture;
            Vec2F textureCoords[4];
            Vec2I size = {1, 1};

        public:
            TextureRegion(const TexturePtr& _texture, const Vec2F& _bottomLeft, const Vec2F& _topRight, const Vec2I& _size = {1, 1});
            TexturePtr getTexture() { return this->texture; }
            const Vec2F* getTextureCoords() { return this->textureCoords; }
            Vec2I getSize() const { return this->size; }

        static TextureRegionPtr create(const TexturePtr& _texture, const Vec2F& _coordinates, const Vec2I & _regionSize, const Vec2I& _numberOfRegions = {1, 1});
    };

}


#endif //ENGINE2_0_TEXTURE_REGION_H
