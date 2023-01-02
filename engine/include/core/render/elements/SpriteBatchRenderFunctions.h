#include "core/render/elements/IRenderizable.h"
#include "glm/glm.hpp"


namespace RDE {

    class Transform;
    class ViewPort;

    struct RenderizableInnerData;

    void calculateGeometryForSpriteRenderer(RenderizableInnerData& data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
    void drawBatchedSpriteRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport);

    void calculateGeometryForTextRenderer(RenderizableInnerData& data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
    void drawBatchedTextRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport);



    void calculateGeometryForUIImage(RenderizableInnerData& data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
    void drawBatchedUIImage(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport);

    void calculateGeometryForUIText(RenderizableInnerData& data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
    void drawBatchedUIText(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport);
}