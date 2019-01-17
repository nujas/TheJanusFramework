// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

import * as vscode from 'vscode';

import * as replace from 'replace-in-file';

import {
	readJson,
	pathExists,
	writeJson,
	copy,
	move
} from 'fs-extra';

import {
	pascalCase
} from 'change-case';

import {
	GetConfiguration,
	GetModuleName,
	GetWorkingPaths,
	GetModuleEntry,
	GetModuleBasePaths
} from './_utilities';

export default function (): vscode.Disposable {
	return vscode.commands.registerCommand('extension.newModule', async () => {
		// The code you place here will be executed every time your command is executed

		//#region Initialize

		const config = GetConfiguration();

		const {
			templateModule
		} = config;

		const workingPaths = await GetWorkingPaths(config);

		if (!workingPaths) {
			return;
		}

		const [
			,
			pluginFilePath,
			sourcePath,
			templateModulePath
		] = workingPaths;

		const newModuleNameRaw = await vscode.window.showInputBox({
			prompt: `Name of the new Module without namespace. If Editor module, please appends "Editor"`,
			placeHolder: `Animation, Core, CoreEditor, QuestEditor . . .`
		});

		if (!newModuleNameRaw) {
			return;
		}

		const newModuleName = GetModuleName(config.namespace, pascalCase(newModuleNameRaw));

		const newModuleSourcePath = `${sourcePath}/${newModuleName}`;

		const newModuleSourceExist = await pathExists(newModuleSourcePath);

		if (newModuleSourceExist) {
			vscode.window.showErrorMessage(`${newModuleName} already exists in Source ! ! ! 💩`);
			return;
		}

		//#endregion 

		//#region Generating
		vscode.window.showWarningMessage(`Generating ${newModuleName} . . . 🎁`);

		const pluginFileJSON = await readJson(pluginFilePath);

		const pluginModules = pluginFileJSON.Modules;

		//#region Checking plugin entry

		const moduleEntryExist = pluginModules.filter(({
			Name
		}: ModuleEntry) => Name === newModuleName);

		if(moduleEntryExist.length !== 0) {
			vscode.window.showErrorMessage(`${newModuleName} already exists in uplugin ! ! ! 💩`);
			return;
		}

		//#endregion

		const newModuleEntry = GetModuleEntry(newModuleName);

		pluginFileJSON.Modules.push(newModuleEntry);

		const writePluginEntryPromise =  writeJson(pluginFilePath, pluginFileJSON, {
			spaces: '\t'
		});

		const copyTemplatePromise = copy(templateModulePath, newModuleSourcePath);

		await Promise.all([writePluginEntryPromise, copyTemplatePromise]);

		const templateModuleBasePaths = GetModuleBasePaths(templateModule, newModuleSourcePath);

		const newModuleBasePaths = GetModuleBasePaths(newModuleName, newModuleSourcePath);

		// Move template path to new path
		await Promise.all(templateModuleBasePaths.map((path, i) => move(path, newModuleBasePaths[i])));

		await replace({
			files: newModuleBasePaths,
			from: new RegExp(templateModule, 'g'),
			to: newModuleName
		});

		//#endregion
		
		//#endregion

		// Display a message box to the user
		vscode.window.showInformationMessage(`Success ! ! ! 🎉`);
	});
}