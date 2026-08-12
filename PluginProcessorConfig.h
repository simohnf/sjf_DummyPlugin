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

#include "sjf/helpers/sjf_OversamplingWrapper.h"
#include "sjf/processors/Reverbs/sjf_ReverbPlayground.h"
#include "sjf/processors/Waveshaper/sjf_Waveshaper.h"
#include "sjf/processors/sjf_Compressor_juce.h"
#include "sjf/processors/sjf_Delay.h"
#include <sjf/helpers/sjf_BypassWrapper.h>
#include <sjf/helpers/sjf_ChunkedWrapper.h>
#include <sjf/helpers/sjf_DummyProcessor.h>
#include <sjf/helpers/sjf_DynamicProcessorSequence.h>
#include <sjf/helpers/sjf_ParameterFactory.h>
#include <sjf/helpers/sjf_PresetManager.h>
#include <sjf/helpers/sjf_ProcessorSequence.h>
#include <sjf/processors/sjf_Filter_juce.h>
#include <sjf/processors/sjf_Limiter_juce.h>
namespace sjf::plugin_processor_config
{
    using namespace sjf::helpers::bypass_wrapper_config;

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

        using Waveshaper = sjf::dsp::waveshaper::FilteredWaveshaper<Saturator>;

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

        using Sat = sjf::helpers::BypassWrapper <Waveshaper,Bypass,Mix>;
        using Del = sjf::helpers::BypassWrapper <Delay,Bypass,Mix>;
        using Rev = sjf::helpers::BypassWrapper <sjf::dsp::Reverb,Bypass,Mix>;
        using Fil = sjf::helpers::BypassWrapper <sjf::dsp::SVF<>,Bypass,Mix>;
        using Comp = sjf::helpers::BypassWrapper <sjf::dsp::Compressor,Bypass, Mix>;

        using Sequence = sjf::helpers::DynamicProcessorSequence<Sat, Del, Rev, Comp, Fil, Fil>;

        // using Seq = sjf::helpers::BypassWrapper <Sequence,Bypass,Mix>;
        using Seq = sjf::helpers::BypassWrapper <sjf::helpers::OversamplingWrapper<Sequence>,Bypass,Mix>;

        using Limit = sjf::helpers::BypassWrapper <sjf::dsp::Limiter,Bypass>;

        // Simply change this alias target to swap out the active core engine
        using Processor = sjf::helpers::ChunkedWrapper  < sjf::helpers::ProcessorSequence<Seq, Limit> >;

        template<typename Processor>
        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(Processor& processor, std::unique_ptr<sjf::helpers::ParameterFactory::GroupMetadata>& groupMetadata) {
            juce::AudioProcessorValueTreeState::ParameterLayout layout;

            using namespace sjf::helpers::processor_sequence;
            using SFC = SubFactoryConfig;


            auto factory = processor.createParameters   ("FX", "FX",
                                                                    NestedConfig{
                                                                        "Chain", "Chain",
                                                                        SFC{"Sat", "Saturator"},
                                                                        SFC{"Del", "Delay"},
                                                                        SFC{"Rev", "Reverb"},
                                                                        SFC{"Comp", "Compressor"},
                                                                        SFC{"Filt1", "Filter"},
                                                                        SFC{"Filt2", "Filter"}
                                                                    },
                                                                    SFC{"Limiter", "Limiter"}
                                                            );
            groupMetadata = std::make_unique<sjf::helpers::ParameterFactory::GroupMetadata>( helpers::ParameterFactory::createMetadataTree(*factory));
            layout.add(std::move(factory));
            return layout;
        }

    };
}
