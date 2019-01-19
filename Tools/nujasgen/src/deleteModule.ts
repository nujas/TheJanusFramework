// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

import * as vscode from 'vscode';

import {
	readJson,
	remove
} from 'fs-extra';

import {
	getConfiguration,
	getWorkingPaths,
	pickExistingModule,
	getModuleIndex,
	prepareModule
} from './_utilities';

export default (): vscode.Disposable => vscode.commands.registerCommand('extension.deleteModule', async () => {
	try {
		const config = getConfiguration();

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

		vscode.window.showWarningMessage(`Deleting ${srcModuleName} . . . 🎁`);

		const srcModuleIndex = await getModuleIndex(pluginModules, srcModuleName);

		pluginFileJSON.Modules.splice(srcModuleIndex, 1);

		await prepareModule(remove, pluginFilePath, pluginFileJSON, [srcModuleSourcePath]);

		vscode.window.showInformationMessage(`Success ! ! ! 🎉`);

	} catch (error) {
		vscode.window.showErrorMessage(error.message);
	}
});