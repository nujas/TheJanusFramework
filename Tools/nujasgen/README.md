# nujasgen README

NujasGen is a tool to generate module for the Janus Framework. It also aids with refactoring and other manual tasks.

## Features

1. Creating new framework module
1. Rename existing module
1. Duplicate existing module
1. Remove existing module

## Requirements

Build: `vsce package --yarn -o ../nujasgen.vsix`

## Extension Settings

This extension contributes the following settings:

* `nujasgen.templateModule`: The template module used to make new one.
* `nujasgen.pluginFile`: The namespace of each module in the current UE4 plugin.
* `nujasgen.namespace`: The name of the UE4 plugin file.

## Known Issues

1. Creating new module with custom template does not handle deep source or header refactoring.

## Release Notes

See CHANGELOG.md

-----------------------------------------------------------------------------------------------------------