#include "core/render/elements/IRenderizable.h"
#include "glm/glm.hpp"
#include "core/graph/components/ui/UIAnchoring.h"


namespace RDE {

    class Transform;
    class ViewPort;

    struct RenderizableInnerData;

	RDE_FUNC void calculateGeometryForSpriteRenderer(RenderizableInnerData& data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
	RDE_FUNC void drawBatchedSpriteRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport);

	RDE_FUNC void calculateGeometryForTextRenderer(RenderizableInnerData& data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
	RDE_FUNC void drawBatchedTextRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport);



	RDE_FUNC void calculateGeometryForUIImage(RenderizableInnerDataUI& data, glm::mat4& _transformMatrix, UIAnchoring* _anchoring, Transform* _transform, const ViewPort* _viewport);
	RDE_FUNC void drawBatchedUIImage(RenderizableInnerDataUI& _data, Batch* _batch, UIAnchoring* _anchoring, Transform* _transform, const ViewPort* _viewport);

	RDE_FUNC void calculateGeometryForUIText(RenderizableInnerDataUI& data, glm::mat4& _transformMatrix, UIAnchoring* _anchoring, Transform* _transform, const ViewPort* _viewport);
	RDE_FUNC void drawBatchedUIText(RenderizableInnerDataUI& _data, Batch* _batch, UIAnchoring* _anchoring, Transform* _transform, const ViewPort* _viewport);
}