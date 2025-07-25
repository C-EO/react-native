/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "CullingContext.h"
#include <react/featureflags/ReactNativeFeatureFlags.h>
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#include <react/renderer/core/LayoutableShadowNode.h>
#include "ShadowViewNodePair.h"

namespace facebook::react {

bool CullingContext::shouldConsiderCulling() const {
  return frame.size.width > 0 && frame.size.height > 0;
}

CullingContext CullingContext::adjustCullingContextIfNeeded(
    const ShadowViewNodePair& pair) const {
  auto cullingContext = *this;
  if (ReactNativeFeatureFlags::enableViewCulling()) {
    if (auto scrollViewShadowNode =
            dynamic_cast<const ScrollViewShadowNode*>(pair.shadowNode)) {
      if (scrollViewShadowNode->getConcreteProps().yogaStyle.overflow() !=
              yoga::Overflow::Visible &&
          !scrollViewShadowNode->getStateData().disableViewCulling) {
        auto layoutMetrics = scrollViewShadowNode->getLayoutMetrics();
        cullingContext.frame.origin =
            -scrollViewShadowNode->getContentOriginOffset(
                /* includeTransform */ true);
        cullingContext.frame.size =
            scrollViewShadowNode->getLayoutMetrics().frame.size;
        cullingContext.transform = Transform::Identity();

        if (layoutMetrics.layoutDirection == LayoutDirection::RightToLeft) {
          // In RTL, content offset is flipped horizontally.
          // We need to flip the culling context frame to match.
          // See:
          // https://github.com/facebook/react-native/blob/c2f39cfdd87c32b9a59efe8a788b8a03f02b0ea0/packages/react-native/React/Fabric/Mounting/ComponentViews/ScrollView/RCTScrollViewComponentView.mm#L579
          auto stateData = scrollViewShadowNode->getStateData();
          cullingContext.frame.origin.x =
              stateData.contentBoundingRect.size.width -
              layoutMetrics.frame.size.width - cullingContext.frame.origin.x;
        }
      } else {
        cullingContext = {};
      }
    } else if (pair.shadowView.traits.check(
                   ShadowNodeTraits::Trait::RootNodeKind)) {
      cullingContext = {};
    } else {
      cullingContext.frame.origin -= pair.shadowView.layoutMetrics.frame.origin;

      if (auto layoutableShadowNode =
              dynamic_cast<const LayoutableShadowNode*>(pair.shadowNode)) {
        cullingContext.transform =
            cullingContext.transform * layoutableShadowNode->getTransform();
      }
    }
  }

  return cullingContext;
}
} // namespace facebook::react
