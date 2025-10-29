// sigma_delta_adc_params.
#include "sigma_delta_adc.hpp"

using namespace std;
using namespace PSAG_LOGGER;

namespace SigmaDeltaADC {
    atomic_bool GLOBAL_SAMPLE_FLAG = false;
    // output (set-p-mtx) params.
    atomic_bool DataSafeShare::GetParamsMtxFlag = false;
    mutex DataSafeShare::GetParamsMtx = {};

    TestOutputParams DataSafeShare::SafeADCOutput = {};
    InternalSmpParams DataSafeShare::SafeADCInternal = {};

    // input (get-p-mtx) params.
    atomic_bool DataSafeShare::SetParamsMtxFlag = false;
    mutex DataSafeShare::SetParamsMtx = {};

    SigmaDeltaParams DataSafeShare::SafeADCSystemParams = {};
    TestInputParams DataSafeShare::SafeADCInput = {};

    atomic_uint64_t DataSafeShare::StepTimerUs = 1000;

    inline AnalogInt SUMACC_PROCESS(
        AnalogInt sumacc, AnalogInt* lastacc, AnalogInt accmax
    ) {
        AnalogInt OutDelta = sumacc >= (*lastacc) ?
            sumacc - (*lastacc) : (accmax - (*lastacc)) + sumacc + 1;
        (*lastacc) = sumacc;
        return OutDelta;
    }

    AnalogFloat SigmaDeltaClaculate::DifferenceAmplifier(
        AnalogFloat vin_p, AnalogFloat vin_n, 
        AnalogFloat gain, AnalogFloat offset, AnalogFloat noise
    ) {
        AnalogFloat DIFF = vin_p - vin_n;
        AnalogFloat VOUT = gain * DIFF + offset;
        return (VOUT += noise);
    }

    AnalogFloat SigmaDeltaClaculate::IntegratorProcess(
        AnalogFloat vin, AnalogFloat* last, AnalogFloat dt,
        AnalogFloat noise
    ) {
        (*last) += vin * dt + noise;
        return (*last);
    }

    BitData SigmaDeltaClaculate::Comparator1BitADC(
        AnalogFloat vin, AnalogFloat vref
    ) {
        return vin >= vref ? (BitData)1 : (BitData)0;
    }

    AnalogFloat SigmaDeltaClaculate::Bipolar1BitDAC(BitData bit, AnalogFloat vref) {
        return bit ? (+vref) : 0.0;
    }

    bool SigmaDeltaClaculate::DigEleAccumulator(
        BitData bit, AnalogInt* sumacc, AnalogInt* lastacc,
        AnalogInt adc_max, AnalogInt adc_acc_max,
        AnalogInt* counter, AnalogInt OCR,
        AnalogInt* output_value
    ) {
        (*sumacc) += bit;
        ++(*counter);
        AnalogInt DeltaValue = 0;
        bool ResultFlag = false;
        if ((*counter) >= OCR) {
            DeltaValue = SUMACC_PROCESS(*sumacc, lastacc, adc_acc_max);
            (*output_value) = adc_max * DeltaValue / OCR;
            (*counter) = 0;
            ResultFlag = true;
        }
        return ResultFlag;
    }

    AnalogInt SigmaDeltaClaculate::DigEleFilter(
        vector<AnalogInt>& dataset, AnalogInt input
    ) {
        AnalogInt AvgValueOut = 0;
        dataset.back() = input;
        for (size_t i = 0; i < dataset.size() - 1; ++i) {
            dataset[i] = dataset[i + 1];
            AvgValueOut += dataset[i + 1];
        }
        return AvgValueOut / (dataset.size() - 1);
    }

    void SigmaDeltaClaculate::SystemFrameUpdate(
        const TestInputParams& in, TestOutputParams* out,
        SigmaDeltaParams& params
    ) {
        AnalogFloat InputVolt = in.TestVoltage + in.TestVoltageNoise;
        AnalogFloat DiffAmp = DifferenceAmplifier(
            in.TestVoltage, DIFF_AMP_VALUE, params.DiffAmpGain, params.DiffAmpOffset,
            GaussGen[0].GetValueWeight(params.DiffAmpNoise)
        );
        ADCInternal.SmpValueInp = in.TestVoltage;
        ADCInternal.SmpValueInn = DIFF_AMP_VALUE;

        AnalogFloat InteProc = IntegratorProcess(
            DiffAmp, &INTER_LAST_VALUE, params.IntegrDt,
            GaussGen[1].GetValueWeight(params.IntegrNoise)
        );
        ADCInternal.SmpValueEuler = InteProc;

        AnalogFloat VoltageRef =
            params.VoltageReference + GaussGen[1].GetValueWeight(params.VoltageRefNoise);
        BitData BitData = Comparator1BitADC(InteProc, VoltageRef);
        DIFF_AMP_VALUE = Bipolar1BitDAC(BitData, VoltageRef);

        ADCInternal.SmpValueVref = VoltageRef;

        // check OCR ´¥·¢¹ýÂËÆ÷¼ÆËã.
        if (DigEleAccumulator(
            BitData, &ACC_HIGH_BITS, &LAST_HIGH_BITS,
            params.SigmaDeltaADCmax, params.HighBitAccMax,
            &OCR_COUNTER, params.SamplerOCR, &out->TestOutputSource
        )) {
            out->TestOutputFilter = 
                DigEleFilter(FilterCache, out->TestOutputSource);
            GLOBAL_SAMPLE_FLAG.store(true);
        }
        ++IntegratorCalcSamples;

        ADCInternal.SmpValueAccBits = ACC_HIGH_BITS;
        ADCInternal.SmpValueCounter = OCR_COUNTER;

        if (ACC_HIGH_BITS >= params.HighBitAccMax)
            ACC_HIGH_BITS = 0;
    }

    void SigmaDeltaClaculate::UpdateParams() {
        unique_lock<mutex> DataLock(SetParamsMtx);
        {
            // data setting update.
            ADCSystemParams = SafeADCSystemParams;
            ADCInput = SafeADCInput;
            // reset state.
            IntegratorCalcSamples = 0;
            ACC_HIGH_BITS    = 0;
            LAST_HIGH_BITS   = 0;
            OCR_COUNTER      = 0;
            DIFF_AMP_VALUE   = 0.0;
            INTER_LAST_VALUE = 0.0;
            // alloc filter cache space.
            FilterCache.resize(ADCSystemParams.FilterCacheSize);
            ADCOutput.TestOutputSource = 0;
            ADCOutput.TestOutputFilter = 0;
        }
        DataLock.unlock();
        SetParamsMtxFlag.store(false);
    }

    void SigmaDeltaClaculate::SampleParams() {
        unique_lock<mutex> DataLock(GetParamsMtx);
        {
            SafeADCOutput = ADCOutput;
            SafeADCInternal = ADCInternal;
        }
        DataLock.unlock();
        GetParamsMtxFlag.store(false);
    }

    void SigmaDeltaClaculate::CalculateSystemInit() {
        FilterCache.resize(8);
    }

    void SigmaDeltaClaculate::CalculateSystemLoop() {
        chrono::microseconds TargetTime = chrono::microseconds(StepTimerUs);
        if ((chrono::system_clock::now() - RunTimer) >= TargetTime) {
            // run sigma-delta adc calculate.
            SystemFrameUpdate(ADCInput, &ADCOutput, ADCSystemParams);
            RunTimer = chrono::system_clock::now();
        }
        // update calc thread params.
        if (SetParamsMtxFlag.load()) UpdateParams();
        // thread sample params.
        if (GetParamsMtxFlag.load()) SampleParams();
    }
}