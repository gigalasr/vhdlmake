# vhdl make

A simple build system made especialy for my uni vhdl projects.

This build system can automaticaly infer the dependencies between vhdl files
and compile only the necerssary files when a or multiple files were changed.

### Usage
```bash
vhdlmake build [entity] - builds project and optionaly elaborates <entity>
vhdlmake run   <entity> - builds project and runs <entity>
vhdlmake info  <entity> - show info for <entity>
```

### Clone and Build
```bash
git clone --recursive https://github.com/gigalasr/vhdlmake.git
cd vhdlmake
mkdir build
cd build
cmake ..
make 
```

If you forgot to clone with ``--recursive`` you can run ``git submodule update --init`` to clone the submodules afterwards.