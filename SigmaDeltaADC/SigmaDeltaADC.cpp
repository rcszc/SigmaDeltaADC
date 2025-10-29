// SigmaDeltaADC, RCSZ, 20250924
// update: 2520924.1506, version: 0.1.2

#include <iostream>
#include "SigmaDeltaADC/system_core/sigma_delta_adc.hpp"

int main() {
	PSAG_LOGGER::SET_PRINTLOG_STATE(true);
	PSAG_LOGGER::SET_PRINTLOG_COLOR(true);
	PSAG_LOGGER_PROCESS::StartLogProcessing("SystemLogs/");

	SystemWindow::SystemWindowRenderer* SystemRenderer = nullptr;
	SigmaDeltaADC::GUI_PANEL_DRAW* GuiComponent1 = new SigmaDeltaADC::GUI_PANEL_DRAW();
	// create init config components.
	SYSTEM_WINDOW_CREATE(
		"Config/window_panel_config.json",
		&SystemRenderer, GuiComponent1
	);
	SystemRenderer->RendererRun();

	PSAG_LOGGER_PROCESS::FreeLogProcessing();
	return 0;
}