//
// Created by Simon Fay on 13/07/2026.
//
#pragma once
#include <sjf/helpers/sjf_ParameterFactory.h>
#include <sjf/helpers/sjf_PresetManager.h>
#include <sjf/helpers/sjf_ChunkedWrapper.h>
#include <sjf/helpers/sjf_ProcessorSequence.h>
#include <sjf/helpers/sjf_BypassWrapper.h>


#include <sjf/helpers/sjf_DummyProcessor.h>

#include "sjf/helpers/sjf_OversamplingWrapper.h"
#include "sjf/processors/sjf_Delay.h"

namespace sjf::plugin_processor_config
{
    struct Config
    {
        using LFO = sjf::dsp::oscillators::lfo::LFO<dsp::oscillators::lfo::DefaultWaveformProvider,
                                                    dsp::oscillators::lfo::config::TempoSync,
                                                    dsp::oscillators::lfo::config::Invert,
                                                    dsp::oscillators::lfo::config::PhaseOffset,
                                                    dsp::oscillators::lfo::config::Smooth,
                                                    dsp::oscillators::lfo::config::Depth>;
        using Delay = sjf::dsp::Delay<  LFO,
                                        dsp::delay_config::TempoSync,
                                        dsp::delay_config::Filter,
                                        dsp::delay_config::Detune,
                                        dsp::delay_config::Saturation,
                                        dsp::delay_config::PingPong,
                                        dsp::delay_config::Link,
                                        dsp::delay_config::LFOTempoSync>;

        // Simply change this alias target to swap out the active core engine
        using Processor = sjf::helpers::ChunkedWrapper  <
                                sjf::helpers::ProcessorSequence <
                                        sjf::helpers::BypassWrapper <
                                                sjf::helpers::OversamplingWrapper<Delay>,
                                                sjf::helpers::bypass_wrapper_config::Bypass,
                                                sjf::helpers::bypass_wrapper_config::Mix
                                                                    >
                                                                >
                                                        >;

        template<typename Processor>
        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(Processor& processor)
        {
            juce::AudioProcessorValueTreeState::ParameterLayout layout;

            // Create the master group
            auto mainGroup = sjf::helpers::ParameterFactory::create ("Parameters", "Parameters");

            using namespace sjf::helpers::processor_sequence;

            layout.add(processor.createParameters   ("", "",
                                                            SubFactoryConfig{"Dummy", "Dummy"}
                                                            // add extra configs for each processor
                                                            )
                                                    );
            return layout;
        }

    };
}
