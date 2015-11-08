/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <gflags/gflags.h>

#include <cstring>

#include "xenia/base/logging.h"
#include "xenia/base/main.h"
#include "xenia/gpu/trace_viewer.h"

// HACK: until we have another impl, we just use gl4 directly.
#include "xenia/gpu/gl4/gl4_command_processor.h"
#include "xenia/gpu/gl4/gl4_graphics_system.h"
#include "xenia/gpu/gl4/gl4_shader.h"

DEFINE_string(target_trace_file, "", "Specifies the trace file to load.");

namespace xe {
namespace gpu {
namespace gl4 {

using namespace xe::gpu::xenos;

class GL4TraceViewer : public TraceViewer {
 public:
  uintptr_t GetColorRenderTarget(uint32_t pitch, MsaaSamples samples,
                                 uint32_t base,
                                 ColorRenderTargetFormat format) override {
    auto command_processor = static_cast<GL4CommandProcessor*>(
        graphics_system_->command_processor());
    return command_processor->GetColorRenderTarget(pitch, samples, base,
                                                   format);
  }

  uintptr_t GetDepthRenderTarget(uint32_t pitch, MsaaSamples samples,
                                 uint32_t base,
                                 DepthRenderTargetFormat format) override {
    auto command_processor = static_cast<GL4CommandProcessor*>(
        graphics_system_->command_processor());
    return command_processor->GetDepthRenderTarget(pitch, samples, base,
                                                   format);
  }

  uintptr_t GetTextureEntry(const TextureInfo& texture_info,
                            const SamplerInfo& sampler_info) override {
    auto command_processor = static_cast<GL4CommandProcessor*>(
        graphics_system_->command_processor());

    auto entry_view =
        command_processor->texture_cache()->Demand(texture_info, sampler_info);
    if (!entry_view) {
      return 0;
    }
    auto texture = entry_view->texture;
    return static_cast<uintptr_t>(texture->handle);
  }
};

int trace_viewer_main(const std::vector<std::wstring>& args) {
  // Grab path from the flag or unnamed argument.
  if (FLAGS_target_trace_file.empty() && args.size() < 2) {
    xe::FatalError("No trace file specified");
    return 1;
  }

  std::wstring path;
  if (!FLAGS_target_trace_file.empty()) {
    // Passed as a named argument.
    // TODO(benvanik): find something better than gflags that supports
    // unicode.
    path = xe::to_wstring(FLAGS_target_trace_file);
  } else {
    // Passed as an unnamed argument.
    path = args[1];
  }
  // Normalize the path and make absolute.
  auto abs_path = xe::to_absolute_path(path);

  GL4TraceViewer trace_viewer;
  if (!trace_viewer.Setup()) {
    xe::FatalError("Unable to setup trace viewer");
    return 1;
  }
  if (!trace_viewer.Load(std::move(abs_path))) {
    xe::FatalError("Unable to load trace file; not found?");
    return 1;
  }
  trace_viewer.Run();
  return 0;
}

}  // namespace gl4
}  // namespace gpu
}  // namespace xe

DEFINE_ENTRY_POINT(L"xenia-gpu-gl4-trace-viewer",
                   L"xenia-gpu-gl4-trace-viewer some.trace",
                   xe::gpu::gl4::trace_viewer_main);
