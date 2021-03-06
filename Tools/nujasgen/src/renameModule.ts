// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

import {
	move,
	readJson
} from 'fs-extra';

import * as vscode from 'vscode';

import {
	getConfiguration,
	getModuleIndex,
	getWorkingPaths,
	inputModuleName,
	pickExistingModule,
	checkModuleExist,
	createModule,
	prepareModule
} from './_utilities';

export default (): vscode.Disposable => vscode.commands.registerCommand('extension.renameModule', async () => {
	try {

		const config = getConfiguration();

		const {
			namespace
		} = config;

		const workingPaths = await getWorkingPaths(config);

		const [,
			pluginFilePath,
			sourcePath
		] = workingPaths;

		const pluginFileJSON = await readJson(pluginFilePath);

		const {
			srcModuleName,
			srcModuleSourcePath,
			pluginModules
		} = await pickExistingModule(pluginFileJSON, sourcePath);

		const [dstModuleName, dstModuleSourcePath] = await inputModuleName(namespace, sourcePath);

		checkModuleExist(pluginModules, dstModuleName);

		vscode.window.showWarningMessage(`Renaming ${srcModuleName} to ${dstModuleName} . . . 🎁`);

		const srcModuleIndex = await getModuleIndex(pluginModules, srcModuleName);
		pluginFileJSON.Modules[srcModuleIndex].Name = dstModuleName;

		await prepareModule(move, pluginFilePath, pluginFileJSON, [srcModuleSourcePath, dstModuleSourcePath]);
		
		await createModule(srcModuleName, dstModuleName, dstModuleSourcePath);

		vscode.window.showInformationMessage(`Success ! ! ! 🎉`);
	} catch (error) {
		vscode.window.showErrorMessage(error.message);
	}
});