/*
███████╗     ██╗███████╗    █████╗ ██╗   ██╗██████╗ ██╗ ██████╗
██╔════╝     ██║██╔════╝   ██╔══██╗██║   ██║██╔══██╗██║██╔═══██╗
███████╗     ██║█████╗     ███████║██║   ██║██║  ██║██║██║   ██║
╚════██║██   ██║██╔══╝     ██╔══██║██║   ██║██║  ██║██║██║   ██║
███████║╚█████╔╝██║███████╗██║  ██║╚██████╔╝██████╔╝██║╚██████╔╝
╚══════╝ ╚════╝ ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═════╝ ╚═╝ ╚═════╝
 */
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
                                                    dsp::oscillators::lfo::lfo_config::TempoSync,
                                                    dsp::oscillators::lfo::lfo_config::Invert,
                                                    dsp::oscillators::lfo::lfo_config::PhaseOffset,
                                                    dsp::oscillators::lfo::lfo_config::Smooth,
                                                    dsp::oscillators::lfo::lfo_config::Depth>;

        using Saturator = sjf::dsp::waveshaper::WaveshaperTypeProvider  <   dsp::waveshaper::SoftClip,
                                                                            dsp::waveshaper::HardClip,
                                                                            dsp::waveshaper::Overdrive,
                                                                            dsp::waveshaper::BucketBrigade,
                                                                            dsp::waveshaper::Tape
                                                                        >;

        using Delay = sjf::dsp::Delay<  LFO,
                                        Saturator,
                                        // dsp::delay_config::Mono,
                                        dsp::delay_config::Feedback,
                                        dsp::delay_config::Offset,
                                        dsp::delay_config::TempoSync,
                                        dsp::delay_config::Filter,
                                        dsp::delay_config::Detune,
                                        dsp::delay_config::PingPong,
                                        dsp::delay_config::Link>;

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

            using namespace sjf::helpers::processor_sequence;

            layout.add(processor.createParameters   ("FX", "FX",
                                                            SubFactoryConfig{"Delay", "Delay"}
                                                            // add extra configs for each processor
                                                            ));
            return layout;
        }

    };
}
