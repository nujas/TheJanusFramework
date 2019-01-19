// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

import * as vscode from 'vscode';

import {
	readJson,
	copy,
} from 'fs-extra';

import {
	getConfiguration,
	getWorkingPaths,
	getModuleEntry,
	inputModuleName,
	checkModuleExist,
	createModule,
	prepareModule
} from './_utilities';

export default (): vscode.Disposable => vscode.commands.registerCommand('extension.newModule', async () => {

	try {
		//#region Initialize

		const config = getConfiguration();

		const {
			namespace,
			templateModule
		} = config;

		const workingPaths = await getWorkingPaths(config);

		const [,
			pluginFilePath,
			sourcePath,
			templateModulePath
		] = workingPaths;

		const [newModuleName, newModuleSourcePath] = await inputModuleName(namespace, sourcePath);

		//#endregion 

		//#region Generating
		vscode.window.showWarningMessage(`Generating ${newModuleName} . . . 🎁`);

		const pluginFileJSON = await readJson(pluginFilePath);

		const pluginModules = pluginFileJSON.Modules;

		//#region Checking plugin entry

		checkModuleExist(pluginModules, newModuleName);

		//#endregion

		const newModuleEntry = getModuleEntry(newModuleName);
		pluginFileJSON.Modules.push(newModuleEntry);

		await prepareModule(copy, pluginFilePath, pluginFileJSON, [templateModulePath, newModuleSourcePath]);

		await createModule(templateModule, newModuleName, newModuleSourcePath);

		//#endregion

		//#endregion
		vscode.window.showInformationMessage(`Success ! ! ! 🎉`);
	} catch (error) {
		vscode.window.showErrorMessage(error.message);
	}
});