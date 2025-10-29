// graphics_window_imgui. RCSZ.
// imgui define maths tools.

#ifndef __GRAPHICS_WINDOW_IMGUI_HPP
#define __GRAPHICS_WINDOW_IMGUI_HPP

//#define IMGUI_ENABLE_FREETYPE
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "implot.h"

#define IMGUI_TEXTURE_ID (ImTextureID)(intptr_t)

#define PSAG_IMVEC_CLAMP(Value, min, max) ((Value) < (min) ? (min) : ((Value) > (max) ? (max) : (Value)))
#define PSAG_IMVEC_PI 3.14159265f
#define PSAG_IMVEC_DEGTORAD(deg) ((deg) * PSAG_IMVEC_PI / 180.0f)

#define IMVEC2_DISTANCE(pos1, pos2) std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y))

#define UI32TOFP32(V) ImU32((V) * 255.0f)
#define IMFP32_CVT_COLU32(R, G, B, A) IM_COL32(ImU32(R * 255.0f), ImU32(G * 255.0f), ImU32(B * 255.0f), ImU32(A * 255.0f))
#define IMVEC4_CVT_COLU32(COL) IM_COL32(UI32TOFP32((COL).x), UI32TOFP32((COL).y), UI32TOFP32((COL).z), UI32TOFP32((COL).w))

#define IMGUI_BOOL_NULLPTR (bool*)NULL
#define IMGUI_TEXTURE_ID (ImTextureID)(intptr_t)
// imgui style-x global spacing.
#define IMGUI_ITEM_SPAC ImGui::GetStyle().ItemSpacing.x

static inline ImVec2 operator+(const ImVec2& v, const ImVec2& s) { return ImVec2(v.x + s.x, v.y + s.y); }
static inline ImVec2 operator-(const ImVec2& v, const ImVec2& s) { return ImVec2(v.x - s.x, v.y - s.y); }
static inline ImVec2 operator*(const ImVec2& v, const ImVec2& s) { return ImVec2(v.x * s.x, v.y * s.y); }
static inline ImVec2 operator/(const ImVec2& v, const ImVec2& s) { return ImVec2(v.x / s.x, v.y / s.y); }

static inline ImVec2 operator+(const ImVec2& v, float s) { return ImVec2(v.x + s, v.y + s); }
static inline ImVec2 operator-(const ImVec2& v, float s) { return ImVec2(v.x - s, v.y - s); }
static inline ImVec2 operator-(float s, const ImVec2& v) { return ImVec2(s - v.x, s - v.y); }
static inline ImVec2 operator*(const ImVec2& v, float s) { return ImVec2(v.x * s, v.y * s); }
static inline ImVec2 operator/(const ImVec2& v, float s) { return ImVec2(v.x / s, v.y / s); }
static inline ImVec2 operator/(float s, const ImVec2& v) { return ImVec2(s / v.x, s / v.y); }

static inline ImVec2& operator+=(ImVec2& v1, const ImVec2& v2) { return v1 = v1 + v2; }
static inline ImVec2& operator-=(ImVec2& v1, const ImVec2& v2) { return v1 = v1 - v2; }
static inline ImVec2& operator*=(ImVec2& v1, const ImVec2& v2) { return v1 = v1 * v2; }
static inline ImVec2& operator/=(ImVec2& v1, const ImVec2& v2) { return v1 = v1 / v2; }

static inline ImVec4 operator+(const ImVec4& v, const ImVec4& s) { return ImVec4(v.x + s.x, v.y + s.y, v.z + s.z, v.w + s.w); }
static inline ImVec4 operator-(const ImVec4& v, const ImVec4& s) { return ImVec4(v.x - s.x, v.y - s.y, v.z - s.z, v.w - s.w); }
static inline ImVec4 operator*(const ImVec4& v, const ImVec4& s) { return ImVec4(v.x * s.x, v.y * s.y, v.z * s.z, v.w * s.w); }
static inline ImVec4 operator/(const ImVec4& v, const ImVec4& s) { return ImVec4(v.x / s.x, v.y / s.y, v.z / s.z, v.w / s.w); }

static inline ImVec4 operator+(const ImVec4& v, float s) { return ImVec4(v.x + s, v.y + s, v.z + s, v.w + s); }
static inline ImVec4 operator-(const ImVec4& v, float s) { return ImVec4(v.x - s, v.y - s, v.z - s, v.w - s); }
static inline ImVec4 operator-(float s, const ImVec4& v) { return ImVec4(s - v.x, s - v.y, s - v.z, s - v.w); }
static inline ImVec4 operator*(const ImVec4& v, float s) { return ImVec4(v.x * s, v.y * s, v.z * s, v.w * s); }
static inline ImVec4 operator/(const ImVec4& v, float s) { return ImVec4(v.x / s, v.y / s, v.z / s, v.w / s); }
static inline ImVec4 operator/(float s, const ImVec4& v) { return ImVec4(s / v.x, s / v.y, s / v.z, s / v.w); }

static inline ImVec4& operator+=(ImVec4& v1, const ImVec4& v2) { return v1 = v1 + v2; }
static inline ImVec4& operator-=(ImVec4& v1, const ImVec4& v2) { return v1 = v1 - v2; }
static inline ImVec4& operator*=(ImVec4& v1, const ImVec4& v2) { return v1 = v1 * v2; }
static inline ImVec4& operator/=(ImVec4& v1, const ImVec4& v2) { return v1 = v1 / v2; }

#define DRAW_CHAR_TEMP_LEN 256

inline void ListDrawTextFmt(
	const ImVec2& position, const ImVec4& color,
	const char* text, ...
) {
	char CharArrayTemp[DRAW_CHAR_TEMP_LEN] = {};
	va_list ParamArgs;
	va_start(ParamArgs, text);
	vsnprintf(CharArrayTemp, DRAW_CHAR_TEMP_LEN, text, ParamArgs);
	va_end(ParamArgs);

	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetWindowPos() + position,
		IMVEC4_CVT_COLU32(color), CharArrayTemp
	);
}

inline void ListDrawCenterTextFmt(
	const ImVec2& position, const ImVec4& color,
	const char* text, ...
) {
	char CharArrayTemp[DRAW_CHAR_TEMP_LEN] = {};
	va_list ParamArgs;
	va_start(ParamArgs, text);
	vsnprintf(CharArrayTemp, DRAW_CHAR_TEMP_LEN, text, ParamArgs);
	va_end(ParamArgs);

	ImVec2 TextSize = ImGui::CalcTextSize(CharArrayTemp);
	ImVec2 OffsetDiff = ImVec2(0.0f, IMGUI_ITEM_SPAC * 0.5f);

	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetWindowPos() + (position - TextSize * 0.5f) + OffsetDiff,
		IMVEC4_CVT_COLU32(color), CharArrayTemp
	);
}

inline void ListDrawCenterText(
	const ImVec2& position, const ImVec4& color, const char* text
) {
	ImVec2 TextSize = ImGui::CalcTextSize(text);
	ImVec2 OffsetDiff = ImVec2(0.0f, IMGUI_ITEM_SPAC * 0.5f);
	ImGui::GetWindowDrawList()->AddText(
		ImGui::GetWindowPos() + (position - TextSize * 0.5f) + OffsetDiff,
		IMVEC4_CVT_COLU32(color), text
	);
}

inline void ListDrawLine(
	const ImVec2& point0, const ImVec2& point1, const ImVec4& color, float wline
) {
	// draw line segment.
	ImGui::GetWindowDrawList()->AddLine(
		ImGui::GetWindowPos() + point0,
		ImGui::GetWindowPos() + point1,
		IMVEC4_CVT_COLU32(color),
		wline
	);
}

inline void ListDrawRectangleFill(
	const ImVec2& position, const ImVec2& size, const ImVec4& color,
	float rounding = 0.0f
) {
	// draw fill rectangle.
	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetWindowPos() + position,
		ImGui::GetWindowPos() + position + size,
		IMVEC4_CVT_COLU32(color), rounding
	);
}

inline void ListDrawTriangleFill(
	const ImVec2& position, const ImVec2& offset1, const ImVec2& offset2, 
	const ImVec4& color
) {
	// draw fill_triangle.
	ImGui::GetWindowDrawList()->AddTriangleFilled(
		ImGui::GetWindowPos() + position,
		ImGui::GetWindowPos() + position + offset1,
		ImGui::GetWindowPos() + position + offset2,
		IMVEC4_CVT_COLU32(color)
	);
}
#endif