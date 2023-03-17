#include "core/render/elements/IRenderizable.h"
#include "glm/glm.hpp"
#include "core/graph/components/ui/UIAnchoring.h"


namespace RDE {

    class Transform;
    class ViewPort;

    struct RenderizableInnerData;

    void calculateGeometryForSpriteRenderer(RenderizableInnerData& data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
    void drawBatchedSpriteRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport);

    void calculateGeometryForTextRenderer(RenderizableInnerData& data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
    void drawBatchedTextRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport);



    void calculateGeometryForUIImage(RenderizableInnerDataUI& data, glm::mat4& _transformMatrix, UIAnchoring* _anchoring, Transform* _transform, const ViewPort* _viewport);
	void drawBatchedUIImage(RenderizableInnerDataUI& _data, Batch* _batch, UIAnchoring* _anchoring, Transform* _transform, const ViewPort* _viewport);

	void calculateGeometryForUIText(RenderizableInnerDataUI& data, glm::mat4& _transformMatrix, UIAnchoring* _anchoring, Transform* _transform, const ViewPort* _viewport);
	void drawBatchedUIText(RenderizableInnerDataUI& _data, Batch* _batch, UIAnchoring* _anchoring, Transform* _transform, const ViewPort* _viewport);
}