// sigma_delta_adc, RCSZ, 20250924
// update: 2520924.1528, version: 0.1.3

#ifndef __SIGMA_DELTA_ADC_H
#define __SIGMA_DELTA_ADC_H
#include <random>
#include "../system_graphics/graphics_window.h"

class GaussianNoise {
private:
	std::mt19937 RandomGen;
	std::normal_distribution<double> NoiseDist;
	double GaussNoiseMean  = 0.0f;
	double GaussNoiseSigma = 0.0f;
public:
	GaussianNoise(double mu = 0.0, double sigma = 1.0)
		: GaussNoiseMean(mu), GaussNoiseSigma(sigma), NoiseDist(mu, sigma)
	{
		std::random_device DeviceRand;
		RandomGen.seed(DeviceRand());
	}

	void SetParamMean(double mu) {
		GaussNoiseMean = mu;
		NoiseDist = std::normal_distribution<double>
			(GaussNoiseMean, GaussNoiseSigma);
	}
	void SetParamSigma(double sigma) {
		GaussNoiseSigma = sigma;
		NoiseDist = std::normal_distribution<double>
			(GaussNoiseMean, GaussNoiseSigma);
	}
	template<typename T>
	T GetValueWeight(T noise_w) { 
		return (T)NoiseDist(RandomGen) * noise_w;
	}
};

namespace SigmaDeltaADC {
	// data type define.
	using AnalogInt = uint64_t;
	using AnalogFloat = long double;
	using BitData = bool;

	struct SigmaDeltaParams {
		AnalogInt HighBitAccBits = 0;
		AnalogInt HighBitAccMax  = 0;

		AnalogInt SamplerOCR = 1;

		AnalogFloat VoltageReference = 0.0f;
		AnalogFloat VoltageRefNoise  = 0.0f;

		AnalogFloat DiffAmpGain   = 0.0f;
		AnalogFloat DiffAmpOffset = 0.0f;
		AnalogFloat DiffAmpNoise  = 0.0f;

		AnalogFloat IntegrDt    = 0.0f;
		AnalogFloat IntegrNoise = 0.0f;

		AnalogInt SigmaDeltaADCbits = 0;
		AnalogInt SigmaDeltaADCmax  = 0;

		size_t FilterCacheSize = 0;
	};

	struct InternalSmpParams {
		AnalogFloat SmpValueEuler = 0.0;
		AnalogFloat SmpValueInp   = 0.0;
		AnalogFloat SmpValueInn   = 0.0;
		AnalogFloat SmpValueVref  = 0.0;

		AnalogInt SmpValueAccBits = 0;
		AnalogInt SmpValueCounter = 0;
	};

	struct TestInputParams {
		AnalogFloat TestVoltage = 0.0f;
		AnalogFloat TestVoltageNoise = 0.0f;
	};

	struct TestOutputParams {
		AnalogInt TestOutputSource = 0;
		AnalogInt TestOutputFilter = 0;
	};

	extern std::atomic_bool GLOBAL_SAMPLE_FLAG;

	class DataSafeShare {
	protected:
		// output (set-p-mtx) params.
		static std::atomic_bool GetParamsMtxFlag;
		static std::mutex GetParamsMtx;

		static TestOutputParams SafeADCOutput;
		static InternalSmpParams SafeADCInternal;

		// input (get-p-mtx) params.
		static std::atomic_bool SetParamsMtxFlag;
		static std::mutex SetParamsMtx;

		static SigmaDeltaParams SafeADCSystemParams;
		static TestInputParams SafeADCInput;

		static std::atomic_uint64_t StepTimerUs;
	};

	class SigmaDeltaClaculate :public DataSafeShare {
	private:
		AnalogInt ACC_HIGH_BITS = 0, LAST_HIGH_BITS = 0;
		AnalogInt OCR_COUNTER = 0;

		std::vector<AnalogInt> FilterCache = {};
		AnalogInt IntegratorCalcSamples = 0;
		AnalogFloat DIFF_AMP_VALUE = 0.0, INTER_LAST_VALUE = 0.0;

		GaussianNoise GaussGen[3] = {};
		// input (get-p-mtx) params.
		SigmaDeltaParams ADCSystemParams = {};
		TestInputParams ADCInput = {};
		// output (set-p-mtx) params.
		TestOutputParams ADCOutput = {};
		InternalSmpParams ADCInternal = {};
	protected:
		// MODULE: 可调差分放大器
		AnalogFloat DifferenceAmplifier(
			AnalogFloat vin_p, AnalogFloat vin_n, AnalogFloat gain,
			AnalogFloat offset, AnalogFloat noise
		);
		// MODULE: Euler积分器
		AnalogFloat IntegratorProcess(
			AnalogFloat vin, AnalogFloat* last, AnalogFloat dt,
			AnalogFloat noise
		);
		// MODULE: 1-Bit ADC, DAC
		BitData Comparator1BitADC(AnalogFloat vin, AnalogFloat vref);
		AnalogFloat Bipolar1BitDAC(BitData bit, AnalogFloat vref);
		// MODULE: 综合数字后端
		bool DigEleAccumulator(
			BitData bit, AnalogInt* sumacc, AnalogInt* lastacc,
			AnalogInt adc_max, AnalogInt adc_acc_max,
			AnalogInt* counter, AnalogInt OCR,
			AnalogInt* output_value
		);
		AnalogInt DigEleFilter(std::vector<AnalogInt>& dataset, AnalogInt input);
		// global calculate update.
		void SystemFrameUpdate(
			const TestInputParams& in, TestOutputParams* out,
			SigmaDeltaParams& params
		);
		// calc step timer.
		std::chrono::system_clock::time_point RunTimer = 
			std::chrono::system_clock::now();
	public:
		void UpdateParams();
		void SampleParams();
		void CalculateSystemInit();
		void CalculateSystemLoop();
	};

	// render sigma-delta gui panel.
	class GUI_PANEL_DRAW :
		public SystemWindow::SystemRenderIntf,
		public SystemWindowFonts::WindowFontsResource,
		public DataSafeShare
	{
	private:
		// input (get-p-mtx) params.
		SigmaDeltaParams ADCSystemParams = {};
		TestInputParams ADCInput = {};
		// output (set-p-mtx) params.
		TestOutputParams ADCOutput = {};
		InternalSmpParams ADCInternal = {};

		AnalogFloat ADC_OUT = 0.0;
		std::vector<float> ValuesCache = {};

		std::vector<float> SampleConstX = {};

		size_t PlotsCacheSize = 512;
		std::vector<float> ADCOutputVolt = {};
		std::vector<float> ADCInputVolt  = {};
	protected:
		int CalcStepTimeUS = 100;
		std::thread* ADCCalculateThread = nullptr;
		std::atomic_bool ThreadClose = false;

		ImVec2 BaseWindowScale = {};
		ImVec2 WCompSize = {};

		void DrawArchWindow();
		void DrawArchFeedBackLine(const ImVec2& w_size);

		void DrawArchCW0(const ImVec2& w_size, const ImVec2& m_size); // 可调差分放大器
		void DrawArchCW1(const ImVec2& w_size, const ImVec2& m_size); // EULER 积分器
		void DrawArchCW2(const ImVec2& w_size, const ImVec2& m_size); // 1BIT ADC
		void DrawArchCW3(const ImVec2& w_size, const ImVec2& m_size); // 1BIT DAC
		void DrawArchCW4(const ImVec2& w_size, const ImVec2& m_size); // 综合数字后端

		void CalculateInitParams();

		void UpdatePlotsCache();
		void UpdateParams();
		void SampleParams();

		void DrawParamsSetting();
		void DrawParamsPlots();
	public:
		bool RenderEventInit(void* config) override;
		void RenderEventLoop() override;
		bool RenderEventFree() override;
	};
}

#endif