//
// Created by Simon Fay on 13/07/2026.
//
#pragma once
#include <sjf/helpers/sjf_ParameterFactory.h>
#include <sjf/helpers/sjf_PresetManager.h>
#include <sjf/helpers/sjf_Chunker.h>
#include <sjf/helpers/sjf_ProcessorSequence.h>
#include <sjf/helpers/sjf_BypassWrapper.h>


#include <sjf/helpers/sjf_DummyProcessor.h>
namespace sjf::plugin_processor_config
{
    struct Config
    {
        // Simply change this alias target to swap out the active core engine
        using Processor = sjf::helpers::Chunker<sjf::helpers::ProcessorSequence<sjf::helpers::BypassWrapper<sjf::Delay>>>;

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