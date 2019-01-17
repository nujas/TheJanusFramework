# The Janus Framework

An Unreal Engine powered framework designed to bootstrap ARPG game with a mid-large open world setting. 

It composes of base classes, pipelines, conventions, and toolings that streamline the process of creating new content in UE4 for said genre.

# Tools
## nujasgen
### Abstract
__NujasGen__ is a VSCode extension created to streamline the process of working with the framework. For more info, read its README.

### Installation

Follow this Stack Overflow answer using the `nujasgen.vsix` file in `Tools`: https://stackoverflow.com/questions/37071388/how-to-install-vscode-extensions-offline

### Usage

1. Open `TheJanusFramework` folder in VSCode
1. Open the `TheJanusFramework.uplugin` file. This file __must be the current active file__ in order to to access `nujasgen` command. 
1. Press `CTRL + SHIFT + P`
1. Search for the desired feature, i.e, try typing `new mod`. It should autocomplete for you. Follow the instruction and it should behave as expected.
