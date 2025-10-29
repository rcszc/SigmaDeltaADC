// sigma_delta_adc.
#include "sigma_delta_adc.hpp"

using namespace std;
using namespace PSAG_LOGGER;

const float BaseWindowSize[2] = { 1280.0f, 720.0f };
namespace SigmaDeltaADC {
	constexpr bool WLINE_EN = true;
	constexpr ImVec4 ConnLineColor = ImVec4(0.0f, 1.0f, 0.72f, 1.0f);

	void GUI_PANEL_DRAW::DrawArchWindow() {
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.0f, 0.42f, 0.8f));

		ImVec2 WindowSize = ImVec2(ImGui::GetIO().DisplaySize * ImVec2(0.95f, 0.6f));
		ImGui::BeginChild("ARCH_VIEW", WindowSize, false);
		ImGui::SetWindowFontScale(BaseWindowScale.x * 1.14f);
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.0f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Text,    ImVec4(1.0f, 0.0f, 1.0f, 1.0f));
			ImVec2 ModuleSize = WindowSize * ImVec2(0.2f, 0.35f);

			DrawArchCW0(WindowSize, ModuleSize * ImVec2(1.0f, 1.11f));
			DrawArchCW1(WindowSize, ModuleSize);
			DrawArchCW2(WindowSize, ModuleSize);
			DrawArchCW3(WindowSize, ModuleSize);
			DrawArchCW4(WindowSize, ModuleSize * ImVec2(1.0f, 1.8f));

			DrawArchFeedBackLine(WindowSize);
			ImGui::PopStyleColor(2);

			ImGui::SetCursorPos(WindowSize * ImVec2(0.01f, 0.828f));
			AnalogFloat OutDiff = (1.0 - (ADCInput.TestVoltage / ADC_OUT)) * 100.0;
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), u8"LOSS: %.7f%%", OutDiff);

			ImGui::SetCursorPos(WindowSize * ImVec2(0.01f, 0.9f));
			ImGui::SetNextItemWidth(BaseWindowScale.x * 142.0f);
			ImGui::InputInt("StepTime(us)", &CalcStepTimeUS);
			CalcStepTimeUS = clamp(CalcStepTimeUS, 1, 20000);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(128.0f);
			ImGui::SliderInt("##SLIDER_TIME", &CalcStepTimeUS, 1, 20000);
			StepTimerUs = CalcStepTimeUS;
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	constexpr ImVec2 LinesList[4] = {
		ImVec2(0.525f, 0.725f), ImVec2(0.125f, 0.725f),
		ImVec2(0.125f, 0.725f), ImVec2(0.125f, 0.489f)
	};
	constexpr ImVec2 LinesListOff[2] = { 
		ImVec2(3.4f, 0.0f), ImVec2(0.0f, 3.4f) 
	};
	void GUI_PANEL_DRAW::DrawArchFeedBackLine(const ImVec2& w_size) {
		ImVec2 Lines0 = w_size * LinesList[1] - LinesListOff[0];
		ImVec2 Lines1 = w_size * LinesList[2] + LinesListOff[1];
		// draw 2-lines.
		ListDrawLine(w_size * LinesList[0], Lines0, ConnLineColor, 7.2f);
		ListDrawLine(Lines1, w_size * LinesList[3], ConnLineColor, 7.2f);
	}

	constexpr ImVec4 TitleTextCol = ImVec4(0.0f, 1.0f, 0.72f, 1.0f);

	// 可调差分放大器
	void GUI_PANEL_DRAW::DrawArchCW0(const ImVec2& w_size, const ImVec2& m_size) {
		ImGui::SetCursorPos(w_size * ImVec2(0.025f, 0.1f));
		ImGui::BeginChild("AIFF_AMP", m_size, WLINE_EN);

		ImGui::SetWindowFontScale(BaseWindowScale.x);

		ListDrawCenterText(m_size * ImVec2(0.5f, 0.09f), TitleTextCol, u8"可调差分放大器");
		ImGui::SetCursorPosY(m_size.y * 0.24f);

		ImGui::Text("Input Volt: %.7f",      ADCInternal.SmpValueInp);
		ImGui::Text("Input VFB: %.3f",       ADCInternal.SmpValueInn);
		ImGui::Text("OpAmp Offset: %.3f",    ADCSystemParams.DiffAmpOffset);
		ImGui::Text("OpAmp Gain: %.3f",      ADCSystemParams.DiffAmpGain);
		ImGui::Text("OpAmp NoiseGain: %.3f", ADCSystemParams.DiffAmpNoise);

		ImGui::EndChild();
	}

	// EULER积分器
	void GUI_PANEL_DRAW::DrawArchCW1(const ImVec2& w_size, const ImVec2& m_size) {
		ListDrawLine(
			w_size * ImVec2(0.225f, 0.275f), w_size * ImVec2(0.275f, 0.275f),
			ConnLineColor, 7.2f
		);
		ImGui::SetCursorPos(w_size * ImVec2(0.275f, 0.1f));
		ImGui::BeginChild("EULER", m_size, WLINE_EN);

		ImGui::SetWindowFontScale(BaseWindowScale.x);

		ListDrawCenterText(m_size * ImVec2(0.5f, 0.09f), TitleTextCol, u8"EULER 积分器");
		ImGui::SetCursorPosY(m_size.y * 0.24f);
		ImGui::Text("Output Value: %.5f",    ADCInternal.SmpValueEuler);
		ImGui::Text("Euler NoiseGain: %.3f", ADCSystemParams.IntegrNoise);

		ImGui::EndChild();
	}

	// 1-Bit ADC
	void GUI_PANEL_DRAW::DrawArchCW2(const ImVec2& w_size, const ImVec2& m_size) {
		ListDrawLine(
			w_size * ImVec2(0.475f, 0.275f), w_size * ImVec2(0.525f, 0.275f),
			ConnLineColor, 7.2f
		);
		ImGui::SetCursorPos(w_size * ImVec2(0.525f, 0.1f));
		ImGui::BeginChild("ADC1BIT", m_size, WLINE_EN);

		ImGui::SetWindowFontScale(BaseWindowScale.x);

		ListDrawCenterText(m_size * ImVec2(0.5f, 0.09f), TitleTextCol, u8"1BIT ADC");
		ImGui::SetCursorPosY(m_size.y * 0.24f);
		ImGui::Text("Input Vref: %.5f", ADCInternal.SmpValueVref);
		ImGui::Text("NoiseGain: %.3f",  ADCSystemParams.VoltageRefNoise);

		ImGui::EndChild();
	}

	// 1-Bit DAC
	void GUI_PANEL_DRAW::DrawArchCW3(const ImVec2& w_size, const ImVec2& m_size) {
		ListDrawLine(
			w_size * ImVec2(0.625f, 0.45f), w_size * ImVec2(0.625f, 0.55f),
			ConnLineColor, 7.2f
		);
		ImGui::SetCursorPos(w_size * ImVec2(0.525f, 0.55f));
		ImGui::BeginChild("DAC1BIT", m_size, WLINE_EN);

		ImGui::SetWindowFontScale(BaseWindowScale.x);

		ListDrawCenterText(m_size * ImVec2(0.5f, 0.09f), TitleTextCol, u8"1BIT DAC");
		ImGui::SetCursorPosY(m_size.y * 0.24f);
		ImGui::Text("Input Vref: %.5f", ADCInternal.SmpValueVref);
		ImGui::Text("NoiseGain: %.3f",  ADCSystemParams.VoltageRefNoise);

		ImGui::EndChild();
	}

	// 综合数字后端
	void GUI_PANEL_DRAW::DrawArchCW4(const ImVec2& w_size, const ImVec2& m_size) {
		ListDrawLine(
			w_size * ImVec2(0.725f, 0.275f), w_size * ImVec2(0.775f, 0.275f),
			ConnLineColor, 7.2f
		);
		ImGui::SetCursorPos(w_size * ImVec2(0.775f, 0.1f));
		ImGui::BeginChild("DIGELE", m_size, WLINE_EN);

		ImGui::SetWindowFontScale(BaseWindowScale.x);

		ListDrawCenterText(m_size * ImVec2(0.5f, 0.05f), TitleTextCol, u8"数字后端");
		ImGui::SetCursorPosY(m_size.y * 0.15f);

		ImGui::Text("Bits ACC: %u",   ADCInternal.SmpValueAccBits);
		ImGui::Text("Counter: %u",    ADCInternal.SmpValueCounter);
		ImGui::Text("Output SRC: %u", ADCOutput.TestOutputSource);

		double Prop = (double)ADCOutput.TestOutputSource / (double)ADCSystemParams.SigmaDeltaADCmax;
		double CalculateVDC = Prop * (double)ADCSystemParams.VoltageReference;
		ImGui::Text("Output VDC: %.8f", CalculateVDC);

		Prop = (double)ADCOutput.TestOutputFilter / (double)ADCSystemParams.SigmaDeltaADCmax;
		CalculateVDC = Prop * (double)ADCSystemParams.VoltageReference;
		ImGui::Text("Output AVG: %.8f", CalculateVDC);

		if (GLOBAL_SAMPLE_FLAG) {
			for (size_t i = 0; i < ValuesCache.size() - 1; ++i)
				ValuesCache[i] = ValuesCache[i + 1];
			ValuesCache.back() = (float)CalculateVDC;
			GLOBAL_SAMPLE_FLAG.store(false);

			if (PlotsCacheSize > ADCOutputVolt.size() && 
				PlotsCacheSize > ADCInputVolt.size()
			) {
				ADCInputVolt.push_back((float)ADCInput.TestVoltage);
				ADCOutputVolt.push_back(ValuesCache.back());
			}
		}
		ADC_OUT = CalculateVDC;

		ImGui::SetCursorPosY(m_size.y * 0.62f);
		ImGui::PlotLines(
			"##Lines", ValuesCache.data(), (int)ValuesCache.size(), 0, (const char*)0,
			3.4028235E38F, 3.4028235E38F, 
			ImVec2(m_size.x - IMGUI_ITEM_SPAC * 2.0f, m_size.y * 0.31f)
		);
		ImGui::EndChild();
	}

	void GUI_PANEL_DRAW::CalculateInitParams() {
		ADCSystemParams.SamplerOCR = 2048;

		ADCSystemParams.HighBitAccBits = 24;
		ADCSystemParams.HighBitAccMax =
			(AnalogInt)pow(2, ADCSystemParams.HighBitAccBits) - 1;

		ADCSystemParams.DiffAmpGain   = 1.0;
		ADCSystemParams.DiffAmpOffset = 0.0;
		ADCSystemParams.DiffAmpNoise  = 0.005;

		ADCSystemParams.IntegrDt    = 0.0001;
		ADCSystemParams.IntegrNoise = 0.0;

		ADCSystemParams.SigmaDeltaADCbits = 16;
		ADCSystemParams.SigmaDeltaADCmax =
			(AnalogInt)pow(2, ADCSystemParams.SigmaDeltaADCbits) - 1;

		ADCSystemParams.VoltageReference = 4.096;
		ADCSystemParams.VoltageRefNoise  = 0.00005;

		ADCSystemParams.FilterCacheSize = 32;
		ADCInput.TestVoltage = 2.345;

		UpdatePlotsCache();
	}

	void GUI_PANEL_DRAW::UpdatePlotsCache() {
		// reset cache stat.
		SampleConstX.clear();
		for (size_t i = 0; i < PlotsCacheSize; ++i)
			SampleConstX.push_back((float)i);
		ADCInputVolt.clear();
		ADCOutputVolt.clear();
	}

	void GUI_PANEL_DRAW::UpdateParams() {
		unique_lock<mutex> DataLock(SetParamsMtx);
		{
			ADCSystemParams.HighBitAccMax =
				(AnalogInt)pow(2, ADCSystemParams.HighBitAccBits) - 1;

			ADCSystemParams.SigmaDeltaADCmax =
				(AnalogInt)pow(2, ADCSystemParams.SigmaDeltaADCbits) - 1;

			SafeADCSystemParams = ADCSystemParams;
			SafeADCInput = ADCInput;
		}
		DataLock.unlock();
		SetParamsMtxFlag.store(true);
	}

	void GUI_PANEL_DRAW::SampleParams() {
		unique_lock<mutex> DataLock(GetParamsMtx);
		{
			ADCOutput = SafeADCOutput;
			ADCInternal = SafeADCInternal;
		}
		DataLock.unlock();
		GetParamsMtxFlag.store(true);
	}

	inline void UILINES() {
		ImGui::Dummy(ImVec2(0.0f, IMGUI_ITEM_SPAC));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, IMGUI_ITEM_SPAC));
	}

	void GUI_PANEL_DRAW::DrawParamsSetting() {
		ImGui::PushStyleColor(ImGuiCol_Text,    ImVec4(0.0f, 1.0f, 0.92f, 0.78f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.0f, 0.42f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.1f, 0.0f, 1.0f));

		ImVec2 WindowSize = ImVec2(ImGui::GetIO().DisplaySize * ImVec2(0.59f, 0.325f));
		ImGui::BeginChild("PARAMS_STTING", WindowSize, false);
		ImGui::SetWindowFontScale(1.0f * BaseWindowScale.x);
		{
			ImGui::Indent(IMGUI_ITEM_SPAC);
			ImGui::Dummy(ImVec2(0.0f, BaseWindowScale.x * IMGUI_ITEM_SPAC * 2.0f));

			int INT_VALUE = (int)ADCSystemParams.SamplerOCR;
			ImGui::SetNextItemWidth(140.0f * BaseWindowScale.x);
			ImGui::InputInt(u8"OCR抽取率", &INT_VALUE);
			ADCSystemParams.SamplerOCR = (AnalogInt)INT_VALUE;

			INT_VALUE = (int)ADCSystemParams.SigmaDeltaADCbits;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(140.0f * BaseWindowScale.x);
			ImGui::InputInt(u8"数字输出位宽", &INT_VALUE);
			ADCSystemParams.SigmaDeltaADCbits = (AnalogInt)INT_VALUE;

			INT_VALUE = (int)ADCSystemParams.HighBitAccBits;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(140.0f * BaseWindowScale.x);
			ImGui::InputInt(u8"累加器位宽", &INT_VALUE);
			ADCSystemParams.HighBitAccBits = (AnalogInt)INT_VALUE;

			UILINES();

			float FP_VALUE = (float)ADCSystemParams.DiffAmpGain;
			ImGui::SetNextItemWidth(100.0f * BaseWindowScale.x);
			ImGui::InputFloat(u8"OpAmp增益", &FP_VALUE, 0.0f, 0.0f, "%.5f");
			ADCSystemParams.DiffAmpGain = (AnalogFloat)FP_VALUE;

			FP_VALUE = (float)ADCSystemParams.DiffAmpOffset;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f * BaseWindowScale.x);
			ImGui::InputFloat(u8"OpAmp偏置", &FP_VALUE, 0.0f, 0.0f, "%.5f");
			ADCSystemParams.DiffAmpOffset = (AnalogFloat)FP_VALUE;

			FP_VALUE = (float)ADCSystemParams.DiffAmpNoise;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f * BaseWindowScale.x);
			ImGui::InputFloat(u8"OpAmp噪声", &FP_VALUE, 0.0f, 0.0f, "%.5f");
			ADCSystemParams.DiffAmpNoise = (AnalogFloat)FP_VALUE;

			UILINES();

			FP_VALUE = (float)ADCSystemParams.VoltageReference;
			ImGui::SetNextItemWidth(100.0f * BaseWindowScale.x);
			ImGui::InputFloat(u8"基准输入", &FP_VALUE, 0.0f, 0.0f, "%.5f");
			ADCSystemParams.VoltageReference = (AnalogFloat)FP_VALUE;

			FP_VALUE = (float)ADCSystemParams.VoltageRefNoise;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f * BaseWindowScale.x);
			ImGui::InputFloat(u8"基准噪声", &FP_VALUE, 0.0f, 0.0f, "%.5f");
			ADCSystemParams.VoltageRefNoise = (AnalogFloat)FP_VALUE;

			FP_VALUE = (float)ADCSystemParams.IntegrDt;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f * BaseWindowScale.x);
			ImGui::InputFloat(u8"积分器步长", &FP_VALUE, 0.0f, 0.0f, "%.5f");
			ADCSystemParams.IntegrDt = (AnalogFloat)FP_VALUE;

			FP_VALUE = (float)ADCSystemParams.IntegrNoise;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.0f * BaseWindowScale.x);
			ImGui::InputFloat(u8"积分器噪声", &FP_VALUE, 0.0f, 0.0f, "%.5f");
			ADCSystemParams.IntegrNoise = (AnalogFloat)FP_VALUE;

			UILINES();

			INT_VALUE = (int)ADCSystemParams.FilterCacheSize;
			ImGui::SetNextItemWidth(140.0f * BaseWindowScale.x);
			ImGui::InputInt(u8"过滤器深度", &INT_VALUE);
			ADCSystemParams.FilterCacheSize = (AnalogInt)INT_VALUE;

			FP_VALUE = (float)ADCInput.TestVoltage;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(108.0f * BaseWindowScale.x);
			ImGui::InputFloat(u8"待测电压", &FP_VALUE);
			ADCInput.TestVoltage = (AnalogFloat)FP_VALUE;

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + IMGUI_ITEM_SPAC);
			ImVec2 ButtonSize = ImVec2(92.0f * BaseWindowScale.x, 24.0f * BaseWindowScale.x);
			if (ImGui::Button(u8"更新参数", ButtonSize))
				UpdateParams();

			INT_VALUE = (int)PlotsCacheSize;
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + IMGUI_ITEM_SPAC);
			ImGui::SetNextItemWidth(140.0f * BaseWindowScale.x);

			if (ImGui::InputInt(u8"PLOTS", &INT_VALUE)) {
				PlotsCacheSize = (AnalogInt)INT_VALUE;
				PlotsCacheSize = PlotsCacheSize < 1 ? 1 : PlotsCacheSize;
				UpdatePlotsCache();
			}
			ImGui::Unindent(IMGUI_ITEM_SPAC);
		}
		ImGui::EndChild();
		ImGui::PopStyleColor(3);
	}

	void GUI_PANEL_DRAW::DrawParamsPlots() {
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.0f, 0.42f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImVec2 WindowSize = ImVec2(ImGui::GetIO().DisplaySize * ImVec2(0.35f, 0.325f));
		ImGui::BeginChild("PARAMS_PLOTS", WindowSize, false);
		ImGui::SetWindowFontScale(1.0f * BaseWindowScale.x);
		{
			if (ImPlot::BeginPlot(u8"SampleCache", WindowSize - ImVec2(0.0f, IMGUI_ITEM_SPAC))) {
				ImPlot::SetupAxes("##TIME", "##VALUES");
				ImPlot::SetupAxesLimits(
					0.0f, (float)PlotsCacheSize, 
					-1.0f, ADCSystemParams.VoltageReference
				);
				// draw input volt plot.
				ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.2f, 0.0f, 0.7f), 2.5f);
				ImPlot::PlotLine(
					"I-Volt", SampleConstX.data(),
					ADCInputVolt.data(), (int)ADCInputVolt.size()
				);
				// draw output volt plot.
				ImPlot::SetNextLineStyle(ImVec4(0.0f, 1.0f, 0.0f, 0.7f), 2.5f);
				ImPlot::PlotLine(
					"O-Volt", SampleConstX.data(),
					ADCOutputVolt.data(), (int)ADCOutputVolt.size()
				);
				ImPlot::EndPlot();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor(2);
	}

	bool GUI_PANEL_DRAW::RenderEventInit(void* config) {
		auto CALC_FUNC = [&]() {
			SigmaDeltaClaculate CalcObject = {};
			CalcObject.CalculateSystemInit();
			while (!ThreadClose) {
				CalcObject.CalculateSystemLoop();
			}
			PushLogger(LogInfo, "CALC_THREAD", "adc calculate thread exit.");
		};
		ADCCalculateThread = new thread(CALC_FUNC);
		PushLogger(LogInfo, "CALC_THREAD", "adc calculate thread create.");
		CalculateInitParams();
		UpdateParams();
		PushLogger(LogInfo, "CALC_THREAD", "update init params.");

		ValuesCache.resize(128);
		return true;
	}

	void GUI_PANEL_DRAW::RenderEventLoop() {
		SampleParams();
		// draw gui system.
		ImVec2 WindowSize(BaseWindowSize[0], BaseWindowSize[1]);
		BaseWindowScale = ImGui::GetIO().DisplaySize / WindowSize;

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,    3.4f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   5.4f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    5.4f);

		ImGui::PushStyleColor(ImGuiCol_WindowBg,      ImVec4(0.0f, 0.0f, 0.0f, 0.32f));
		ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(0.0f, 1.0f, 0.92f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.38f, 0.0f, 0.92f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.08f, 0.0f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.08f, 0.0f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg,       ImVec4(0.0f, 0.1f, 0.12f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_PlotLines,     ImVec4(0.0f, 1.0f, 1.0f, 1.0f));

		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		// main window attributes.
		ImGuiWindowFlags WindowFlags =
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;
		ImGui::Begin("MAIN_WINDOW", (bool*)0, WindowFlags);
		{
			ImVec2 DAW = ImGui::GetIO().DisplaySize * ImVec2(0.025f, 0.025f);
			ImVec2 DPS = ImGui::GetIO().DisplaySize * ImVec2(0.025f, 0.65f);
			ImVec2 DPP = ImGui::GetIO().DisplaySize * ImVec2(0.625f, 0.65f);

			ImGui::PushFont(FindFonts("wqdkwm"));
			{
				ImGui::SetCursorPos(DAW);
				DrawArchWindow();
				ImGui::SetCursorPos(DPS);
				DrawParamsSetting();
				ImGui::SetCursorPos(DPP);
				DrawParamsPlots();
			}
			ImGui::PopFont();
		}
		ImGui::End();
		ImGui::PopStyleColor(7);
		ImGui::PopStyleVar(4);
	}

	bool GUI_PANEL_DRAW::RenderEventFree() {
		ThreadClose.store(true);
		ADCCalculateThread->join();
		delete ADCCalculateThread;
		PushLogger(LogInfo, "CALC_THREAD", "thread object free.");
		return true;
	}
}