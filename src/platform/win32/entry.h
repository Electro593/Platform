/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _GRAPHICS _OPENGL || _DIRECTX

#define FONTS_DIR    "assets\\fonts\\"
#define SHADERS_DIR  "assets\\shaders\\"
#define TEXTURES_DIR "assets\\textures\\"

struct platform_module {
    c08 *FileName;
    win32_module DLL;
    datetime LastWriteTime;
    
    func_Module_Load   *Load;
    func_Module_Init   *Init;
    func_Module_Update *Update;
    func_Module_Unload *Unload;
    
    vptr ModuleData;
};