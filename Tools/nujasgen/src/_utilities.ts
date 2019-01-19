import {
    upper,
    pascalCase
} from 'change-case';

import {
    workspace,
    window
} from 'vscode';

import {
    move,
    pathExists,
    writeJson
} from 'fs-extra';

import * as replace from 'replace-in-file';

export function getConfiguration(): NujasGenConfig {
    return <any > workspace.getConfiguration('nujasgen');
}

export function getProjectRoot() {
    if (workspace.workspaceFolders) {
        return workspace.workspaceFolders[0].uri.fsPath;
    } else {
        return null;
    }
}

export function getPluginPaths({
    pluginFile,
    templateModule
}: NujasGenConfig) {
    const projectRoot = getProjectRoot();
    if (!projectRoot) {
        throw Error('No workspace openned ! ! ! 💩');
    }
    return [
        projectRoot,
        `${projectRoot}/${pluginFile}`,
        `${projectRoot}/Source`,
        `${projectRoot}/Source/${templateModule}`,
    ];
}

export function getModuleName(namespace: string, rawName: string) {
    return pascalCase(`${namespace}${rawName}`);
}

export function getHeaderNameSpace(moduleName: string) {
    return upper(`${moduleName}_API`);
}

export async function getWorkingPaths(config: NujasGenConfig) {
    const pluginPaths = getPluginPaths(config);

    const [, pluginFilePath,,templateModulePath] = pluginPaths!;


    if (!pathExists(pluginFilePath)) {
        throw Error(`No ${config.pluginFile} found ! ! ! 💩`);
    }

    if (!pathExists(templateModulePath)) {
        throw Error(`No ${config.templateModule} module found in Source ! ! ! 💩`);
    }
    return pluginPaths;
}

export function getModuleBasePaths(moduleName: string, moduleSourcePath: string) {
    return getModuleBaseFiles(moduleName).map(fileName => `${moduleSourcePath}/${fileName}`);
}

export function getModuleBaseFiles(moduleName: string) {
    return [
        `${moduleName}.Build.cs`,
        `Private/${moduleName}Module.cpp`,
        `Public/${moduleName}Module.h`,
    ];
}

export function getModuleEntry(moduleName: string, type = "Runtime", loadingPhase = "Default"): ModuleEntry {
    return {
        "Name": moduleName,
        "Type": type,
        "LoadingPhase": loadingPhase
    };
}


export function renameHeaderNamespace(srcModuleName: string, dstModuleName: string, dstModuleSourcePath: string) {
    const srcHeaderNamespace = getHeaderNameSpace(srcModuleName);
    const dstHeaderNamespace = getHeaderNameSpace(dstModuleName);
    // Replace header namespace
    return replace({
        files: `${dstModuleSourcePath}/**`,
        from: new RegExp(srcHeaderNamespace, 'g'),
        to: dstHeaderNamespace
    });
}

export async function refactorModuleBaseFiles(srcModuleName: string, dstModuleName: string, dstModuleSourcePath: string) {

    const srcModuleBasePaths = getModuleBasePaths(srcModuleName, dstModuleSourcePath);
    const dstModuleBasePaths = getModuleBasePaths(dstModuleName, dstModuleSourcePath);

    await Promise.all(srcModuleBasePaths.map((path, i) => move(path, dstModuleBasePaths[i])));
    // Replace module name
    await replace({
        files: dstModuleBasePaths,
        from: new RegExp(srcModuleName, 'g'),
        to: dstModuleName
    });
}

export async function inputModuleName(namespace: string, sourcePath: string) {
    const dstModuleNameRaw = await window.showInputBox({
        prompt: `Name of the new Module without namespace. If Editor module, please appends "Editor"`,
        placeHolder: `Animation, Core, CoreEditor, QuestEditor . . .`
    });

    if (!dstModuleNameRaw) {
        throw Error(`A name is required . . . 👻`);
    }

    const dstModuleName = getModuleName(namespace, pascalCase(dstModuleNameRaw));

    const dstModuleSourcePath = `${sourcePath}/${dstModuleName}`;

    if (await pathExists(dstModuleSourcePath)) {
        throw Error(`${dstModuleName} already exists in Source ! ! ! 💩`);
    }

    return [dstModuleName, dstModuleSourcePath];
}

export function getModuleIndex(pluginModules: Array < ModuleEntry > , srcModuleName: string) {
    const index = pluginModules.findIndex(({
        Name
    }) => Name === srcModuleName);

    if (index === -1) {
        throw Error(`${srcModuleName} does not exist in uplugin ! ! ! 💩`);
    }

    return index;
}

export function checkModuleExist(pluginModules: Array < ModuleEntry > , srcModuleName: string) {
    const moduleEntryExist = pluginModules.filter(({
        Name
    }: ModuleEntry) => Name === srcModuleName);

    if (moduleEntryExist.length !== 0) {
        throw Error(`${srcModuleName} already exists in uplugin ! ! ! 💩`);
    }
}

export async function pickExistingModule(pluginFileJSON: any, sourcePath: string) {

    const pluginModules = < Array < ModuleEntry >> pluginFileJSON.Modules;

    const pluginModuleNames = pluginModules.map(({
        Name
    }) => Name);

    const srcModuleName = await window.showQuickPick(pluginModuleNames);

    if (!srcModuleName) {
        throw Error(`A name is required . . . 👻`);
    }

    const srcModuleSourcePath = `${sourcePath}/${srcModuleName}`;

    if (!await (pathExists(srcModuleSourcePath))) {
        throw Error(`${srcModuleName} does not exist in Source ! ! ! 💩`);
    }

    return {
        srcModuleName,
        srcModuleSourcePath,
        pluginModules
    };
}

export async function createModule(srcModuleName: string, dstModuleName: string, dstModuleSourcePath: string) {
	await refactorModuleBaseFiles(srcModuleName, dstModuleName, dstModuleSourcePath);
	await renameHeaderNamespace(srcModuleName, dstModuleName, dstModuleSourcePath);
}

export async function prepareModule(pathOperation: Function, pluginFilePath: string, pluginFileJSON: any, modulePaths: Array<String>) {
    const writePluginEntryPromise = writeJson(pluginFilePath, pluginFileJSON, {
		spaces: '\t'
	});
	const pathOperationPromise = pathOperation.apply(null, modulePaths);
	await Promise.all([
		writePluginEntryPromise,
		pathOperationPromise
	]);
}