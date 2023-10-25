#include <base/source/fstreamer.h>
#include <pluginterfaces/base/fplatform.h>
#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/main/pluginfactory.h>
#include <public.sdk/source/vst/vstaudioeffect.h>
#include <public.sdk/source/vst/vstaudioprocessoralgo.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <vstgui/plugin-bindings/vst3editor.h>

// Example project based on tutorial
// Implements gain parameter, param change events, opt. aux input for sidechaining
// https://steinbergmedia.github.io/vst3_dev_portal/pages/Tutorials/Code+your+first+plug-in.html#part-1-coding-your-plug-in

#define FULL_VERSION_STR       "1.0.0.0"
#define stringPluginName       "VST3Test"
#define stringOriginalFilename "VST3Test.vst3"
#define stringFileDescription  "VST3Test VST3"
#define stringCompanyName      "Deez Nutz Ltd."
#define stringCompanyURL       "deeznuts.com"
#define stringCompanyEmail     "compile@deeznuts.com"
#define stringLegalCopyright   "Copyright(c) 2023 ."
#define stringLegalTrademarks  "VST is a trademark of Steinberg Media Technologies GmbH"
#define VST3TestVST3Category   Steinberg::Vst::PlugType::kFxTools

// Make sure you generate new sets of UUIDs for you own plugin.
// https://guidgenerator.com/
static const Steinberg::FUID kVST3TestProcessorUID(0x13253970, 0x860055F6, 0xAE96C583, 0xF1C9E39A);
static const Steinberg::FUID kVST3TestControllerUID(0x67B44A1F, 0xEEBD5DCF, 0x8526EA52, 0x26EEDC33);

enum GainParams : Steinberg::Vst::ParamID
{
    kParamGainId,
};

//------------------------------------------------------------------------
//  VST3TestController
//------------------------------------------------------------------------
struct VST3TestController : Steinberg::Vst::EditControllerEx1
{
    VST3TestController()           = default;
    ~VST3TestController() override = default;

    static Steinberg::FUnknown* createInstance(void* /*context*/)
    {
        return (Steinberg::Vst::IEditController*)new VST3TestController;
    }

    // IPluginBase
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override
    {
        Steinberg::tresult result = EditControllerEx1::initialize(context);
        if (result != Steinberg::kResultOk)
            return result;

        parameters.addParameter(
            STR16("Gain"),
            STR16("dB"),
            0,
            .5,
            Steinberg::Vst::ParameterInfo::kCanAutomate,
            GainParams::kParamGainId,
            0);

        return result;
    }
    Steinberg::tresult PLUGIN_API terminate() override { return EditControllerEx1::terminate(); }

    // EditController
    Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream* state) override
    {
        // Here, you get the state of the component (Processor part)
        if (! state)
            return Steinberg::kResultFalse;

        Steinberg::IBStreamer streamer(state, kLittleEndian);

        float savedParam1 = 0.0f;
        if (streamer.readFloat(savedParam1) == false)
            return Steinberg::kResultFalse;

        // sync with our parameter
        if (Steinberg::Vst::Parameter* param = parameters.getParameter(GainParams::kParamGainId))
            param->setNormalized(savedParam1);

        return Steinberg::kResultOk;
    }

    // Here the Host wants to open your editor (if you have one)
    Steinberg::IPlugView* PLUGIN_API createView(Steinberg::FIDString name) override
    {
        // if (Steinberg::FIDStringsEqual(name, Steinberg::Vst::ViewType::kEditor))
        //     return new VSTGUI::VST3Editor(this, "view", "editor.uidesc");
        return nullptr;
    }

    Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override { return Steinberg::kResultTrue; }
    Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override { return Steinberg::kResultTrue; }

    // called by host to update your parameters
    Steinberg::tresult PLUGIN_API
    setParamNormalized(Steinberg::Vst::ParamID tag, Steinberg::Vst::ParamValue value) override
    {
        Steinberg::tresult result = EditControllerEx1::setParamNormalized(tag, value);
        return result;
    }
    // called by host to get string for normalized value of a specific parameter
    Steinberg::tresult PLUGIN_API getParamStringByValue(
        Steinberg::Vst::ParamID    tag,
        Steinberg::Vst::ParamValue valueNormalized,
        Steinberg::Vst::String128  string) override
    {
        return EditControllerEx1::getParamStringByValue(tag, valueNormalized, string);
    }

    Steinberg::tresult PLUGIN_API getParamValueByString(
        Steinberg::Vst::ParamID     tag,
        Steinberg::Vst::TChar*      string,
        Steinberg::Vst::ParamValue& valueNormalized) override
    {
        // called by host to get a normalized value from a string representation of a specific parameter
        // (without having to set the value!)
        return EditControllerEx1::getParamValueByString(tag, string, valueNormalized);
    }

    //---Interface---------
    DEFINE_INTERFACES
    // Here you can add more supported VST3 interfaces
    // DEF_INTERFACE (Vst::IXXX)
    END_DEFINE_INTERFACES(EditController)
    DELEGATE_REFCOUNT(EditController)
};

//------------------------------------------------------------------------
//  VST3TestProcessor
//------------------------------------------------------------------------
// https://steinbergmedia.github.io/vst3_doc/vstsdk/classSteinberg_1_1Vst_1_1AudioEffect.html
// https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IAudioProcessor.html
struct VST3TestProcessor : public Steinberg::Vst::AudioEffect
{
    VST3TestProcessor() { Steinberg::Vst::AudioEffect::setControllerClass(kVST3TestControllerUID); }
    ~VST3TestProcessor() override {}

    // Create function
    static Steinberg::FUnknown* createInstance(void* /*context*/)
    {
        return (Steinberg::Vst::IAudioProcessor*)new VST3TestProcessor;
    }

    //--- ---------------------------------------------------------------------
    // AudioEffect overrides:
    //--- ---------------------------------------------------------------------
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override
    {
        Steinberg::tresult result = AudioEffect::initialize(context);
        if (result != Steinberg::kResultOk)
            return result;

        //--- create Audio IO ------
        addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
        addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

        /* If you don't need an event bus, you can remove the next line */
        //---create Event In/Out busses (1 bus with only 1 channel)------
        addEventInput(STR16("Event In"), 1);

        // create a Mono SideChain input bus
        addAudioInput(STR16("Mono Aux In"), Steinberg::Vst::SpeakerArr::kMono, Steinberg::Vst::kAux, 0);

        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API terminate() override { return AudioEffect::terminate(); }

    /** Switch the Plug-in on/off */
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override { return AudioEffect::setActive(state); }

    Steinberg::tresult PLUGIN_API setBusArrangements(
        Steinberg::Vst::SpeakerArrangement* inputs,
        Steinberg::int32                    numIns,
        Steinberg::Vst::SpeakerArrangement* outputs,
        Steinberg::int32                    numOuts) override
    {
        // the first input is the Main Input and the second is the SideChain Input
        // be sure that we have 2 inputs and 1 output
        if (numIns == 2 && numOuts == 1)
        {
            // we support only when Main input has the same number of channel than the output
            if (Steinberg::Vst::SpeakerArr::getChannelCount(inputs[0]) !=
                Steinberg::Vst::SpeakerArr::getChannelCount(outputs[0]))
                return Steinberg::kResultFalse;

            // we are agree with all arrangement for Main Input and output
            // then apply them
            getAudioInput(0)->setArrangement(inputs[0]);
            getAudioOutput(0)->setArrangement(outputs[0]);

            // Now check if sidechain is mono (we support in our example only mono Side-chain)
            if (Steinberg::Vst::SpeakerArr::getChannelCount(inputs[1]) != 1)
                return Steinberg::kResultFalse;

            // OK the Side-chain is mono, we accept this by returning kResultTrue
            return Steinberg::kResultTrue;
        }

        // we do not accept what the host wants: return kResultFalse !
        return Steinberg::kResultFalse;
    }

    /** Will be called before any process call */
    Steinberg::tresult PLUGIN_API setupProcessing(Steinberg::Vst::ProcessSetup& newSetup) override
    {
        return Steinberg::Vst::AudioEffect::setupProcessing(newSetup);
    }

    /** Asks if a given sample size is supported see SymbolicSampleSizes. */
    Steinberg::tresult PLUGIN_API canProcessSampleSize(Steinberg::int32 symbolicSampleSize) override
    {
        return symbolicSampleSize == Steinberg::Vst::kSample32 ? Steinberg::kResultTrue : Steinberg::kResultFalse;
    }
    // Process audio
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override;

    // load plugin state
    Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override
    {
        if (! state)
            return Steinberg::kResultFalse;

        // called when we load a preset or project, the model has to be reloaded
        Steinberg::IBStreamer streamer(state, kLittleEndian);
        float                 savedParam1 = 0.f;
        if (streamer.readFloat(savedParam1) == false)
            return Steinberg::kResultFalse;
        mGain = savedParam1;

        return Steinberg::kResultOk;
    }
    // save plugin state
    Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override
    {
        // here we need to save the model (preset or project)
        float                 toSaveParam1 = mGain;
        Steinberg::IBStreamer streamer(state, kLittleEndian);
        streamer.writeFloat(toSaveParam1);
        return Steinberg::kResultOk;
    }

    Steinberg::Vst::ParamValue mGain          = 1.0;
    Steinberg::Vst::ParamValue mGainReduction = 0.0;
};

//------------------------------------------------------------------------
// VST3TestProcessor
//------------------------------------------------------------------------

Steinberg::tresult PLUGIN_API VST3TestProcessor::process(Steinberg::Vst::ProcessData& data)
{
    //--- First: Read inputs parameter changes-----------
    if (data.inputParameterChanges)
    {
        // for each parameter defined by its ID
        Steinberg::int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
        for (Steinberg::int32 index = 0; index < numParamsChanged; index++)
        {
            // for this parameter we could iterate the list of value changes (could 1 per audio block or more!)
            // in this example, we get only the last value (getPointCount - 1)
            Steinberg::Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(index);
            if (paramQueue)
            {
                Steinberg::Vst::ParamValue value;
                Steinberg::int32           sampleOffset;
                Steinberg::int32           numPoints = paramQueue->getPointCount();
                switch (paramQueue->getParameterId())
                {
                case GainParams::kParamGainId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == Steinberg::kResultTrue)
                        mGain = value;
                    break;
                }
            }
        }
    }

    //---Second: Read input events-------------
    // get the list of all event changes
    Steinberg::Vst::IEventList* eventList = data.inputEvents;
    if (eventList)
    {
        Steinberg::int32 numEvent = eventList->getEventCount();
        for (Steinberg::int32 i = 0; i < numEvent; i++)
        {
            Steinberg::Vst::Event event;
            if (eventList->getEvent(i, event) == Steinberg::kResultOk)
            {
                // here we do not take care of the channel info of the event
                switch (event.type)
                {
                case Steinberg::Vst::Event::kNoteOnEvent:
                    // use the velocity as gain modifier: a velocity max (1) will lead to silent audio
                    mGainReduction = event.noteOn.velocity; // value between 0 and 1
                    break;

                case Steinberg::Vst::Event::kNoteOffEvent:
                    // noteOff reset the gain modifier
                    mGainReduction = 0.f;
                    break;
                }
            }
        }
    }

    //-- Flush case: we only need to update parameter, noprocessing possible
    if (data.numInputs == 0 || data.numSamples == 0)
        return Steinberg::kResultOk;

    //--- Here, you have to implement your processing
    Steinberg::int32 numChannels = data.inputs[0].numChannels;

    //---get audio buffers using helper-functions(vstaudioprocessoralgo.h)-------------
    Steinberg::uint32 sampleFramesSize = getSampleFramesSizeInBytes(processSetup, data.numSamples);
    void**            in               = getChannelBuffersPointer(processSetup, data.inputs[0]);
    void**            out              = getChannelBuffersPointer(processSetup, data.outputs[0]);

    // Here could check the silent flags
    //---check if silence---------------
    // normally we have to check each channel (simplification)
    if (data.inputs[0].silenceFlags != 0)
    {
        // mark output silence too
        data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

        // the plug-in has to be sure that if it sets the flags silence that the output buffer are clear
        for (Steinberg::int32 i = 0; i < numChannels; i++)
        {
            // do not need to be cleared if the buffers are the same (in this case input buffer are
            // already cleared by the host)
            if (in[i] != out[i])
                memset(out[i], 0, sampleFramesSize);
        }
        // nothing to do at this point
        return Steinberg::kResultOk;
    }

    float gain = mGain - mGainReduction;
    if (gain < 0.f) // gain should always positive or zero
        gain = 0.f;

    void** auxIn = nullptr;

    // Check if the Side-chain input is activated
    bool auxActive = false;
    if (getAudioInput(1)->isActive())
    {
        auxIn     = getChannelBuffersPointer(processSetup, data.inputs[1]);
        auxActive = true;
    }
    if (auxActive)
    {
        // for each channel (left and right)
        for (Steinberg::int32 i = 0; i < numChannels; i++)
        {
            Steinberg::int32          samples = data.numSamples;
            Steinberg::Vst::Sample32* ptrIn   = (Steinberg::Vst::Sample32*)in[i];
            Steinberg::Vst::Sample32* ptrOut  = (Steinberg::Vst::Sample32*)out[i];
            // Side-chain is mono, so take auxIn[0]: index 0
            Steinberg::Vst::Sample32* ptrAux = (Steinberg::Vst::Sample32*)auxIn[0];
            Steinberg::Vst::Sample32  tmp;

            // for each sample in this channel
            while (--samples >= 0)
            {
                // apply modulation and gain
                tmp         = (*ptrIn++) * (*ptrAux++) * gain;
                (*ptrOut++) = tmp;
            }
        }
    }
    else
    {
        // for each channel (left and right)
        for (Steinberg::int32 i = 0; i < numChannels; i++)
        {
            Steinberg::int32          samples = data.numSamples;
            Steinberg::Vst::Sample32* ptrIn   = (Steinberg::Vst::Sample32*)in[i];
            Steinberg::Vst::Sample32* ptrOut  = (Steinberg::Vst::Sample32*)out[i];
            Steinberg::Vst::Sample32  tmp;
            // for each sample in this channel
            while (--samples >= 0)
            {
                // apply gain
                tmp         = (*ptrIn++) * gain;
                (*ptrOut++) = tmp;
            }
        }
    }

    return Steinberg::kResultOk;
}

//------------------------------------------------------------------------
//  VST Plug-in Entry
//------------------------------------------------------------------------
// Windows: do not forget to include a .def file in your project to export
// GetPluginFactory function!
//------------------------------------------------------------------------

BEGIN_FACTORY_DEF(stringCompanyName, stringCompanyURL, stringCompanyEmail)

//---First Plug-in included in this factory-------
// its kVstAudioEffectClass component
DEF_CLASS2(
    INLINE_UID_FROM_FUID(kVST3TestProcessorUID),
    Steinberg::PClassInfo::kManyInstances, // cardinality
    kVstAudioEffectClass,                  // the component category (do not changed this)
    stringPluginName,                      // here the Plug-in name (to be changed)
    Steinberg::Vst::kDistributable, // means that component and controller could be distributed on different computers
    Steinberg::Vst::PlugType::kFx,  // Subcategory for this Plug-in (to be changed)
    FULL_VERSION_STR,               // Plug-in version (to be changed)
    kVstVersionString,              // the VST 3 SDK version (do not changed this, use always this define)
    VST3TestProcessor::createInstance) // function pointer called when this component should be instantiated

// its kVstComponentControllerClass component
DEF_CLASS2(
    INLINE_UID_FROM_FUID(kVST3TestControllerUID),
    Steinberg::PClassInfo::kManyInstances, // cardinality
    kVstComponentControllerClass,          // the Controller category (do not changed this)
    stringPluginName "Controller",         // controller name (could be the same than component name)
    0,                                     // not used here
    "",                                    // not used here
    FULL_VERSION_STR,                      // Plug-in version (to be changed)
    kVstVersionString,                     // the VST 3 SDK version (do not changed this, use always this define)
    VST3TestController::createInstance)    // function pointer called when this component should be instantiated

//----for others Plug-ins contained in this factory, put like for the first Plug-in different DEF_CLASS2---

END_FACTORY

#include "vst3_scu.cpp"
