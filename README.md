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
