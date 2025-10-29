// graphics_window. RCSZ.
// version: 25_09_17, update: 25_09_17

#ifndef __GRAPHICS_WINDOW_H
#define __GRAPHICS_WINDOW_H
// WINDOWS API.
#if defined(_WIN32)
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#pragma comment(lib, "setupapi.lib")
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")
#endif

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freetype.lib")

#include <array>
#include <queue>
#include <functional>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h>

#include "nlohmann/json.hpp"

#define GLOBAL_CONFIG_TITLE "SigmaDeltaADC - RCSZ"
#define GLOBAL_CONFIG_LOGO  ""

#define GRAPH_WINDOW_ENABLE_WINAPI true
#define GRAPH_WINDOW_FIXED_SIZE    false

#include "../system_logger/psag_system_logger.hpp"
#include "graphics_window_imgui.hpp"

namespace SystemWindowAnim {
	// anim function components.
	class WindowAnimComp {
	private:
		std::function<void()> AnimComponents = {};
	protected:
		bool SystemAnimLoad();
		void SystemAnimPlayer();
	};
}

namespace SystemWindow {
	class SystemWindowRenderer;
}
namespace SystemWindowFonts {
	// global imgui fonts(resource) pointer.
	class WindowFontsResource {
	private:
		static std::unordered_map<std::string, ImFont*> FontsResource;
		bool RegisterFonts(const std::string& name, ImFont* font);
	protected:
		ImFont* FindFonts(const std::string& name);
		friend class SystemWindow::SystemWindowRenderer;
	};
}
namespace SystemWindow {
	// render system error code flags.
#define STARTER_GRAPH_STATE_NOERR    0
#define STARTER_GRAPH_STATE_NULLPTR -2
#define STARTER_GRAPH_STATE_ICON    -3
#define STARTER_GRAPH_STATE_O_INIT  -4
#define STARTER_GRAPH_STATE_O_FREE  -5

	struct SystemWindowFont {
		// font file system path, font gloabl size.
		std::string FontFilePath   = {};
		float       FontFileSize   = 1.0f;
		std::string FontUniqueName = {};

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			SystemWindowFont,
			FontFilePath, FontFileSize, FontUniqueName
		)
		// REGISTER NLOHMANN JSON.
	};

	// renderer start init config.
	struct SystemWindowConifg {
		uint32_t WindowInitSizeW = 0;
		uint32_t WindowInitSizeH = 0;

		bool     WindowVsync = false;
		uint32_t WindowMSAA  = false;

		std::vector<SystemWindowFont> ImGuiFonts = {};
		std::string ImGuiShaderVersion = {};

		std::array<float, 4> ExtWinStyleBorder = {};
		std::array<float, 4> ExtWinStyleColor  = {};

		bool ExtWinStyleEnableBlur = false;
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			SystemWindowConifg,
			WindowInitSizeW, WindowInitSizeH,
			WindowVsync, WindowMSAA,
			ImGuiShaderVersion, ImGuiFonts,
			ExtWinStyleBorder, ExtWinStyleColor,
			ExtWinStyleEnableBlur
		)
		// REGISTER NLOHMANN JSON.
	};

	// renderer draw gui interface.
	class SystemRenderIntf {
	public:
		virtual bool RenderEventInit(void* config) = 0;
		virtual void RenderEventLoop() = 0;
		virtual bool RenderEventFree() = 0;
	};

	class SystemWindowRenderer :
		public SystemWindowFonts::WindowFontsResource,
		public SystemWindowAnim::WindowAnimComp
	{
	protected:
		SystemRenderIntf* GuiDrawObject = nullptr;
		GLFWwindow* WindowObject = nullptr;
		// component global status code. 
		int32_t ErrorCode = STARTER_GRAPH_STATE_NOERR;

		bool InitCreateOpenGL(ImVec2 size, bool vsync, uint32_t msaa, const std::string& name);
		bool InitCreateImGui(const char* shader, const std::vector<SystemWindowFont>& fonts);

		void FreeRendererContext();
	public:
		SystemWindowRenderer(
			const std::string& name, const ImVec2& size, bool vsync, uint32_t msaa,
			const std::string& shader, const std::vector<SystemWindowFont>& fonts
		);
		~SystemWindowRenderer();

		void SettingColorBackground (const ImVec4& color);
		void SettingColorBorder     (const ImVec4& color);
		void SettingTransparencyBlur();

		void SettingGuiDrawObject(
			SystemRenderIntf* object, void* exten = nullptr
		);
		void SettingGuiIconFiles(const std::string& image);
		// run framework renderer.
		int32_t RendererRun();
	};
}

// system window object create.
void SYSTEM_WINDOW_CREATE(
	const std::string& config,
	SystemWindow::SystemWindowRenderer** renderer, 
	SystemWindow::SystemRenderIntf* comp
);
#endif