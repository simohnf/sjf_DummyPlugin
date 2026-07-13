# Dummy Project

Simple dummy juce project for use when creating plugins.

----

## To create a new Plugin

----

On Mac
```shell
rm -rf DummyPlugin
git clone https://github.com/simohnf/sjf_DummyPlugin.git
chmod 744 sjf_DummyPlugin/updateProjectName.sh
./sjf_DummyPlugin/updateProjectName.sh
```

You can then edit the ```PluginProcessorConfig.h``` file to set the Processor/Sequence to be used and the relative ids and names to be used for parameter groups 
