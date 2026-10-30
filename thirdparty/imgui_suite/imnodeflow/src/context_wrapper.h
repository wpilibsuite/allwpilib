#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <limits>

inline static void CopyIOEvents(ImGuiContext* src, ImGuiContext* dst, ImVec2 origin, float scale)
{
    dst->PlatformImeData = src->PlatformImeData;
    dst->IO.DeltaTime = src->IO.DeltaTime;

    // Intentionally copy InputEventsTrail (last frame's already-processed events)
    // rather than InputEventsQueue (this frame's pending events). Copying the queue
    // would cause every event to be processed twice — once by the outer context,
    // once here — resulting in duplicated inputs. The trade-off is exactly one
    // frame of input latency inside the inner context.
    dst->InputEventsQueue = src->InputEventsTrail;
    for (ImGuiInputEvent& e : dst->InputEventsQueue) {
        if (e.Type == ImGuiInputEventType_MousePos) {
            e.MousePos.PosX = (e.MousePos.PosX - origin.x) / scale;
            e.MousePos.PosY = (e.MousePos.PosY - origin.y) / scale;
        }
    }
}

// ---------------------------------------------------------------------------
// AppendDrawData
//
// Blits one inner-context draw list into the outer window's draw list,
// transforming vertex positions by (scale, origin) and adjusting all offsets
// so the appended commands are valid in the outer buffer's index space.
//
// Must be called with the outer context active — ImGui::GetIO() inside this
// function reads the OUTER context's BackendFlags, which is correct because
// ContainedContext::end() restores the outer context before calling this.
//
// The caller (end()) pre-reserves VtxBuffer and IdxBuffer on the outer draw
// list using draw_data->TotalVtxCount / TotalIdxCount before the loop, so
// the resize() calls here will not trigger reallocs.
//
// _VtxCurrentIdx semantics (critical):
//   ImGui asserts _VtxCurrentIdx < (1<<16) after every primitive when using
//   16-bit indices. This value tracks vertices in the CURRENT segment only
//   (since the last VtxOffset boundary), NOT the total outer buffer size.
//   It must be set to the segment-relative vertex count, not an absolute
//   outer-buffer position, which could exceed 65535.
// ---------------------------------------------------------------------------
inline static void AppendDrawData(ImDrawList* src, ImVec2 origin, float scale)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Early exit if buffers empty
    if (src->VtxBuffer.empty() || src->CmdBuffer.empty()) {
        return;
    }

    const bool hasVtxOffset = (ImGui::GetIO().BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset) != 0;

    // Extend destination buffers and transform vertices into place.
    //   VtxBuffer and IdxBuffer were pre-reserved in end() so these resize()
    //   calls should not realloc in the common case.
    const unsigned int vtx_start = static_cast<unsigned int>(dl->VtxBuffer.Size);
    const unsigned int idx_start = static_cast<unsigned int>(dl->IdxBuffer.Size);

    dl->VtxBuffer.resize(dl->VtxBuffer.Size + src->VtxBuffer.Size);
    dl->IdxBuffer.resize(dl->IdxBuffer.Size + src->IdxBuffer.Size);
    dl->CmdBuffer.reserve(dl->CmdBuffer.Size + src->CmdBuffer.Size);

    {
        ImDrawVert*       dst_v = dl->VtxBuffer.Data + vtx_start;
        const ImDrawVert* src_v = src->VtxBuffer.Data;
        for (int i = 0; i < src->VtxBuffer.Size; ++i) {
            dst_v[i].uv  = src_v[i].uv;
            dst_v[i].col = src_v[i].col;
            dst_v[i].pos = src_v[i].pos * scale + origin;
        }
    }

    // Copy indices and fixup commands.
    ImDrawIdx* dst_idx_base = dl->IdxBuffer.Data + idx_start;

    if (hasVtxOffset)
    {
        // Hot path: all modern backends (DX11/12, Vulkan, Metal, GL3+).

        // Indices are segment-relative and require no per-index arithmetic —
        // bulk copy the entire index buffer in one shot, then fix up cmd
        // offsets in the command loop. This uses a single SIMD-optimised memcpy
        // instead of a scalar loop.
        memcpy(dst_idx_base, src->IdxBuffer.Data, src->IdxBuffer.Size * sizeof(ImDrawIdx));

        // Cache for segment boundary scan: ImGui emits commands in non-decreasing
        // VtxOffset order, so consecutive commands often share the same segment.
        // Recomputing the forward scan per command would be O(n^2); caching the
        // result per unique VtxOffset keeps it O(n).
        unsigned int cached_vtx_offset    = UINT_MAX;
        unsigned int cached_seg_vtx_count = 0;

        for (int ci = 0; ci < src->CmdBuffer.Size; ++ci) {
            ImDrawCmd cmd = src->CmdBuffer[ci];

            cmd.ClipRect.x = cmd.ClipRect.x * scale + origin.x;
            cmd.ClipRect.y = cmd.ClipRect.y * scale + origin.y;
            cmd.ClipRect.z = cmd.ClipRect.z * scale + origin.x;
            cmd.ClipRect.w = cmd.ClipRect.w * scale + origin.y;

            // Compute the vertex count for this segment so _VtxCurrentIdx
            // stays segment-relative (never exceeds 65535 with 16-bit indices).
            // Skip the scan when this command shares a VtxOffset with the
            // previous one — same segment, boundary already known.
            if (cmd.VtxOffset != cached_vtx_offset) {
                cached_vtx_offset = cmd.VtxOffset;
                unsigned int next_vtx_offset = static_cast<unsigned int>(src->VtxBuffer.Size);
                for (int ni = ci + 1; ni < src->CmdBuffer.Size; ++ni) {
                    if (src->CmdBuffer[ni].VtxOffset > cmd.VtxOffset) {
                        next_vtx_offset = src->CmdBuffer[ni].VtxOffset;
                        break;
                    }
                }
                cached_seg_vtx_count = next_vtx_offset - cmd.VtxOffset;
            }

            // Segment-relative count keeps the ImGui 16-bit index assert happy.
            dl->_VtxCurrentIdx = cached_seg_vtx_count;

            cmd.VtxOffset += vtx_start;
            cmd.IdxOffset += idx_start;
            dl->CmdBuffer.push_back(cmd);
        }
    }
    else {
        // Cold path: Legacy backends without RendererHasVtxOffset (OpenGL 2.x / ES2).

        // Bake the vertex offset into each index to produce absolute outer-buffer
        // indices, since these backends cannot use cmd.VtxOffset to shift the base.
        const ImDrawIdx* src_idx_base = src->IdxBuffer.Data;

        for (auto cmd : src->CmdBuffer) { // Note: cmd is a local copy
            IM_ASSERT(cmd.VtxOffset == 0 && "Non-zero VtxOffset in legacy path; backend flag mismatch. Should not happen.");

            // Adjust clipping
            cmd.ClipRect.x = cmd.ClipRect.x * scale + origin.x;
            cmd.ClipRect.y = cmd.ClipRect.y * scale + origin.y;
            cmd.ClipRect.z = cmd.ClipRect.z * scale + origin.x;
            cmd.ClipRect.w = cmd.ClipRect.w * scale + origin.y;

            const unsigned int base = vtx_start + cmd.VtxOffset;
            // Verify the baked indices will fit in ImDrawIdx, handles both 16 and 32-bit indices.
            IM_ASSERT(  (sizeof(ImDrawIdx) >= 4 ||
                        base + static_cast<unsigned int>(src->VtxBuffer.Size) - 1u
                        <= static_cast<unsigned int>(std::numeric_limits<ImDrawIdx>::max()))
                        && "Vertex count exceeds ImDrawIdx range; enable RendererHasVtxOffset or use 32-bit indices");

            const ImDrawIdx* si = src_idx_base + cmd.IdxOffset;
            ImDrawIdx*       di = dst_idx_base  + cmd.IdxOffset;
            for (unsigned int ii = 0; ii < cmd.ElemCount; ++ii) {
                di[ii] = static_cast<ImDrawIdx>(si[ii] + base);
            }
            cmd.VtxOffset  = 0;
            cmd.IdxOffset += idx_start;
            dl->CmdBuffer.push_back(cmd);
        }

        // Guaranteed safe by the IM_ASSERT above.
        dl->_VtxCurrentIdx = vtx_start + static_cast<unsigned int>(src->VtxBuffer.Size);
    }

    // Advance write pointers to the new buffer ends.
    // _VtxCurrentIdx was already set inside each path above.
    dl->_VtxWritePtr = dl->VtxBuffer.Data + dl->VtxBuffer.Size;
    dl->_IdxWritePtr = dl->IdxBuffer.Data + dl->IdxBuffer.Size;
}

struct ContainedContextConfig
{
    bool extra_window_wrapper = false;
    ImVec2 size = {0.f, 0.f};
    ImU32 color = IM_COL32_WHITE;
    bool zoom_enabled = true;
    float zoom_min = 0.3f;
    float zoom_max = 2.f;
    float zoom_divisions = 10.f;
    float zoom_smoothness = 5.f;
    float default_zoom = 1.f;
    ImGuiKey reset_zoom_key = ImGuiKey_R;
    ImGuiMouseButton scroll_button = ImGuiMouseButton_Middle;
    // Per-frame veto on canvas panning, set by the host before end(). Useful
    // when scroll_button is remapped to Left, where panning has to share the
    // button with node dragging, link drag-out and selection: the host can set
    // this from ImNodeFlow::isNodeDragged() / isLinkDragging() / "anything
    // selected" so those gestures win over the pan.
    bool block_scroll = false;
};

class ContainedContext
{
public:
    ~ContainedContext();
    ContainedContextConfig& config() { return m_config; }
    void begin();
    void end();
    [[nodiscard]] ImVec2 size() const { return m_size; }
    [[nodiscard]] float scale() const { return m_scale; }
    [[nodiscard]] const ImVec2& origin() const { return m_origin; }
    [[nodiscard]] bool hovered() const { return m_hovered; }
    [[nodiscard]] const ImVec2& scroll() const { return m_scroll; }
    [[nodiscard]] ImVec2 getScreenDelta() { return m_original_ctx->IO.MouseDelta / scale(); }
    ImGuiContext* getRawContext() { return m_ctx; }
    void setFontDensity();
private:
    ContainedContextConfig m_config;

    ImVec2 m_origin;
    ImVec2 m_pos;
    ImVec2 m_size;
    ImGuiContext* m_ctx = nullptr;
    ImGuiContext* m_original_ctx = nullptr;

    bool m_anyWindowHovered = false;
    bool m_anyItemActive = false;
    bool m_hovered = false;

    // Font rasterizer density of the *host* context, sampled in begin() right
    // after BeginChild() (i.e. after ImGui has set it from the host viewport's
    // FramebufferScale). 1.0 on a standard-DPI display, 2.0 on a typical HiDPI
    // one. The inner context has no platform window of its own, so ImGui can
    // only ever derive 1.0 for it -- we have to carry the host's value across.
    float m_hostFontDensity = 1.f;

    float m_scale = m_config.default_zoom, m_scaleTarget = m_config.default_zoom;
    ImVec2 m_scroll = {0.f, 0.f};
};

inline ContainedContext::~ContainedContext()
{
    if (m_ctx) ImGui::DestroyContext(m_ctx);
}

// Targets whichever context is current at call time.
// In begin(), this is called twice: once for the outer context's child window
// (so the outer renderer rasterizes at the correct density), and once inside
// the inner context's Begin() when extra_window_wrapper is enabled.
//
// The density we want is (host display density * canvas zoom): the zoom factor
// alone is only correct on a 1x display. The inner context is created without a
// platform backend, so its viewport FramebufferScale is 0 and its
// DisplayFramebufferScale is 1 -- ImGui's own SetCurrentWindow() therefore
// derives a density of 1.0 for it no matter what the host runs at. Multiplying
// in the host's density keeps node text as crisp as the rest of the app on
// HiDPI displays.
inline void ContainedContext::setFontDensity()
{
#if IMGUI_VERSION_NUM >= 19198
    const float density = m_hostFontDensity * m_scale;
    ImGui::SetFontRasterizerDensity(roundf(density * 100.0f) / 100.0f); // Round density to two digits.
#endif
}

inline void ContainedContext::begin()
{
    ImGui::PushID(this);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, m_config.color);
    ImGui::BeginChild("view_port", m_config.size, 0, ImGuiWindowFlags_NoMove);
    // BeginChild() -> SetCurrentWindow() has just reset the density from the
    // host viewport's FramebufferScale, so this reads the host's true display
    // density. Sample it before setFontDensity() overwrites it.
#if IMGUI_VERSION_NUM >= 19198
    m_hostFontDensity = ImGui::GetFontRasterizerDensity();
    if (m_hostFontDensity <= 0.f)
        m_hostFontDensity = 1.f;
#endif
    // Set font density on the OUTER context's child window so the outer renderer
    // rasterizes fonts at the correct scale before we switch context below.
    setFontDensity();
    ImGui::PopStyleColor();
    m_pos = ImGui::GetWindowPos();

    m_size = ImGui::GetContentRegionAvail();
    m_origin = ImGui::GetCursorScreenPos();
    m_original_ctx = ImGui::GetCurrentContext();
    const ImGuiStyle& orig_style = ImGui::GetStyle();
    if (!m_ctx) m_ctx = ImGui::CreateContext(ImGui::GetIO().Fonts);
    ImGui::SetCurrentContext(m_ctx);
    ImGuiStyle& new_style = ImGui::GetStyle();
    new_style = orig_style;

    CopyIOEvents(m_original_ctx, m_ctx, m_origin, m_scale);

    ImGui::GetIO().DisplaySize = m_size / m_scale;
    ImGui::GetIO().ConfigInputTrickleEventQueue = false;

    // Copy backend flags so the inner context matches the outer renderer's
    // capabilities. This includes RendererHasVtxOffset (enables the optimised
    // AppendDrawData path) and RendererHasTextures (must match for texture IDs
    // to be interpreted correctly).
    ImGui::GetIO().ConfigFlags  = m_original_ctx->IO.ConfigFlags;
    ImGui::GetIO().BackendFlags = m_original_ctx->IO.BackendFlags;
#ifdef IMGUI_HAS_VIEWPORT
    // Viewport and docking features require the platform backend to cooperate;
    // strip them from the inner context which has no platform window of its own.
    ImGui::GetIO().ConfigFlags &= ~(ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable);
#endif

    ImGui::NewFrame();

    if (!m_config.extra_window_wrapper)
        return;
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("viewport_container", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove
                                                | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    // Set font density again now inside the inner context.
    setFontDensity();
    ImGui::PopStyleVar();
}

inline void ContainedContext::end()
{
    m_anyWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
    if (m_config.extra_window_wrapper && ImGui::IsWindowHovered())
        m_anyWindowHovered = false;

    // Not simply IsAnyItemActive(): clicking empty space in a window makes
    // ImGui set ActiveId to that window's MoveId, and it does so even for
    // windows flagged ImGuiWindowFlags_NoMove -- see the comment in
    // StartMouseMovingWindow(), the id is kept alive purely so that dragging
    // away from the window does not light up hover on other windows. Our
    // viewport container is NoMove, so a plain IsAnyItemActive() reports "an
    // item is active" for the whole duration of any left-press on empty
    // canvas. That is harmless for the default middle-button pan, but it
    // silently kills panning whenever scroll_button is remapped to Left.
    // Treat a bare MoveId as "nothing active" so left-drag can pan, while a
    // genuinely held widget (button, slider, drag-float in a node) still
    // blocks it.
    {
        ImGuiContext& g = *ImGui::GetCurrentContext();
        m_anyItemActive = g.ActiveId != 0 &&
                          !(g.ActiveIdWindow && g.ActiveIdWindow->MoveId == g.ActiveId);
    }

    if (m_config.extra_window_wrapper)
        ImGui::End();

    ImGui::Render();

    ImDrawData* draw_data = ImGui::GetDrawData();

    m_original_ctx->PlatformImeData = m_ctx->PlatformImeData;
    ImGui::SetCurrentContext(m_original_ctx);
    m_original_ctx = nullptr;

    // Pre-reserve outer draw list buffers using the total counts from the inner
    // draw data. This prevents repeated reallocs inside AppendDrawData when
    // there are multiple CmdLists to blit.
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->VtxBuffer.reserve(dl->VtxBuffer.Size + draw_data->TotalVtxCount);
        dl->IdxBuffer.reserve(dl->IdxBuffer.Size + draw_data->TotalIdxCount);
    }

    // AppendDrawData is called with the outer context active, so ImGui::GetIO()
    // inside it correctly reads the outer context's BackendFlags.
    for (int i = 0; i < draw_data->CmdListsCount; ++i)
        AppendDrawData(draw_data->CmdLists[i], m_origin, m_scale);

    m_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && !m_anyWindowHovered;

    // Zooming
    if (m_config.zoom_enabled && m_hovered && ImGui::GetIO().MouseWheel != 0.f)
    {
        m_scaleTarget += ImGui::GetIO().MouseWheel / m_config.zoom_divisions;
        m_scaleTarget = m_scaleTarget < m_config.zoom_min ? m_config.zoom_min : m_scaleTarget;
        m_scaleTarget = m_scaleTarget > m_config.zoom_max ? m_config.zoom_max : m_scaleTarget;

        if (m_config.zoom_smoothness == 0.f)
        {
            m_scroll += (ImGui::GetMousePos() - m_pos) / m_scaleTarget - (ImGui::GetMousePos() - m_pos) / m_scale;
            m_scale = m_scaleTarget;
        }
    }
    // Guard against zoom_smoothness == 0: dividing by zero yields +inf, making
    // the threshold comparison always false — correct by accident but fragile.
    if (m_config.zoom_smoothness > 0.f &&
        abs(m_scaleTarget - m_scale) >= 0.015f / m_config.zoom_smoothness)
    {
        float cs = (m_scaleTarget - m_scale) / m_config.zoom_smoothness;
        m_scroll += (ImGui::GetMousePos() - m_pos) / (m_scale + cs) - (ImGui::GetMousePos() - m_pos) / m_scale;
        m_scale += (m_scaleTarget - m_scale) / m_config.zoom_smoothness;

        if (abs(m_scaleTarget - m_scale) < 0.015f / m_config.zoom_smoothness)
        {
            m_scroll += (ImGui::GetMousePos() - m_pos) / m_scaleTarget - (ImGui::GetMousePos() - m_pos) / m_scale;
            m_scale = m_scaleTarget;
        }
    }

    // Zoom reset
    if (ImGui::IsKeyPressed(m_config.reset_zoom_key, false))
        m_scaleTarget = m_config.default_zoom;

    // Scrolling
    if (!m_config.block_scroll && m_hovered && !m_anyItemActive && ImGui::IsMouseDragging(m_config.scroll_button, 0.f))
    {
        m_scroll += ImGui::GetIO().MouseDelta / m_scale;
    }

    // Update inner context MousePos for the NEXT frame's input. ImGui reads
    // MousePos at NewFrame(), so writing it here (end of this frame) is correct.
    this->m_ctx->IO.MousePos = (ImGui::GetMousePos() - m_origin) / m_scale;
    ImGui::EndChild();
    ImGui::PopID();
}
