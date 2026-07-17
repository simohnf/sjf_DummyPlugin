# Dummy Project

Simple dummy juce project for use when creating plugins.

Currently demonstrates a maximal Delay example with:
- Filtering
- Saturation
- Modulation
- Detune
- Oversampling


----

## To create a new Plugin

----

On Mac

Open Terminal and go to the directory in which you want to. save the project, e.g.
```shell
cd ~/Desktop
```
Then, using https
```shell
rm -rf DummyPlugin
git clone https://github.com/simohnf/sjf_DummyPlugin.git
chmod 744 sjf_DummyPlugin/updateProjectName.sh
./sjf_DummyPlugin/updateProjectName.sh
```
or using ssh
```shell
rm -rf DummyPlugin
git clone git@github.com:simohnf/sjf_DummyPlugin.git
chmod 744 sjf_DummyPlugin/updateProjectName.sh
./sjf_DummyPlugin/updateProjectName.sh
```


You can then edit the ```PluginProcessorConfig.h``` file to set the Processor/Sequence to be used and the relative ids and names to be used for parameter groups 
