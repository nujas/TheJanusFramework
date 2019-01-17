import {
    upper,
    pascalCase
} from 'change-case';

import {
    workspace,
    window
} from 'vscode';
import {
    pathExists
} from 'fs-extra';

export function GetConfiguration() : NujasGenConfig {
    return <any>workspace.getConfiguration('nujasgen');
}

export function GetProjectRoot() {
    if (workspace.workspaceFolders) {
        return workspace.workspaceFolders[0].uri.fsPath;
    } else {
        return null;
    }
}

export function GetPluginPaths({pluginFile, templateModule} : NujasGenConfig) {
    const projectRoot = GetProjectRoot();
    if (!projectRoot) {
        return null;
    }
    return [
        projectRoot,
        `${projectRoot}/${pluginFile}`,
        `${projectRoot}/Source`,
        `${projectRoot}/Source/${templateModule}`,
    ];
}

export function GetModuleName(namespace: string, rawName: string) {
    return pascalCase(`${namespace}${rawName}`);
}

export function GetHeaderNameSpace(namespace: string, moduleName: string) {
    return upper(`${namespace}${moduleName}_API`);
}

export async function GetWorkingPaths(config : NujasGenConfig) {

    const pluginPaths = GetPluginPaths(config);

    if (!pluginPaths) {
        window.showErrorMessage('No workspace openned! 💩');
        return null;
    }

    const [, pluginFilePath] = pluginPaths;

    const pathValid = await pathExists(pluginFilePath);

    if (!pathValid) {
        window.showErrorMessage(`No ${config.pluginFile} found! 💩`);
        return null;
    }

    return pluginPaths;
}

export function GetModuleBasePaths(moduleName: string, moduleSourcePath: string) {
    return GetModuleBaseFiles(moduleName).map(fileName => `${moduleSourcePath}/${fileName}`);
}

export function GetModuleBaseFiles(moduleName: string) {
    return [
        `${moduleName}.Build.cs`,
        `Private/${moduleName}Module.cpp`,
        `Public/${moduleName}Module.h`,
    ];
}

export function GetModuleEntry(moduleName: string, type = "Runtime", loadingPhase = "Default"): ModuleEntry {
    return {
        "Name": moduleName,
        "Type": type,
        "LoadingPhase": loadingPhase
    };
}