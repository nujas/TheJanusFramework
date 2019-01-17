declare module 'replace-in-file';
type ModuleEntry = {
    Name : string,
    Type: string,
    LoadingPhase: string
};

type NujasGenConfig = {
    templateModule: string,
    pluginFile: string, 
    namespace: string
}