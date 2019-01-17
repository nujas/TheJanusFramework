// Copyright 2018-2019 The Janus Project | 2034 Complex LLC. All Rights Reserved.

import * as vscode from 'vscode';

export default function () : vscode.Disposable {
   return vscode.commands.registerCommand('extension.renameModule', () => {
		// The code you place here will be executed every time your command is executed
		
		// Display a message box to the user
		vscode.window.showInformationMessage('Rename existing plugin module!');
	});
}
