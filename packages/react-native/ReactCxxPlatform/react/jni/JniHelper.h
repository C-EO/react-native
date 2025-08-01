/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <fbjni/Context.h>
#include <fbjni/fbjni.h>

namespace facebook::react {

jobject getApplication(JNIEnv* env);

jni::alias_ref<jni::AContext> getContext();

} // namespace facebook::react
