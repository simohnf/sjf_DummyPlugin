```
███████╗     ██╗███████╗    █████╗ ██╗   ██╗██████╗ ██╗ ██████╗
██╔════╝     ██║██╔════╝   ██╔══██╗██║   ██║██╔══██╗██║██╔═══██╗
███████╗     ██║█████╗     ███████║██║   ██║██║  ██║██║██║   ██║
╚════██║██   ██║██╔══╝     ██╔══██║██║   ██║██║  ██║██║██║   ██║
███████║╚█████╔╝██║███████╗██║  ██║╚██████╔╝██████╔╝██║╚██████╔╝
╚══════╝ ╚════╝ ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═════╝ ╚═╝ ╚═════╝
```

# Dummy Project

Simple dummy juce project for use when creating plugins.

Currently demonstrates an oversampled audio chain containing:

- A dynamic processing chain with:
    - Delay with modulation, saturation, filtering
    - Reverb 
    - Waveshaper
    - 2 * Filter
- Limiter


----

## To create a new Plugin

On Mac

Open Terminal and go to the directory in which you want to. save the project, e.g.
```shell
cd ~/Desktop
```
Then, using https
```shell
rm -rf sjf_DummyPlugin
git clone https://github.com/simohnf/sjf_DummyPlugin.git
python3 sjf_DummyPlugin/Setup/initialise_project.py
```
or using ssh
```shell
rm -rf sjf_DummyPlugin
git clone git@github.com:simohnf/sjf_DummyPlugin.git
python3 sjf_DummyPlugin/Setup/initialise_project.py
```


You can then edit the ```PluginProcessorConfig.h``` file to set the Processor/Sequence to be used and the relative ids and names to be used for parameter groups 


----

