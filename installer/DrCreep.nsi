;	The Castles of Dr. Creep
; 	-------------------------------
; 
;	Copyright (C) 2009-2010 Robert Crossfield
;  
;	
;
; $Id$
; 
;
; NSIS Installer script

!include revInst.nsh

SetCompressor lzma
;--------------------------------
;Include Modern UI

  !include MUI2.nsh
  !include LogicLib.nsh
  
;--------------------------------
;General
	;XPStyle on
	
	RequestExecutionLevel admin
	Name	"Dr. Creep"
	Caption "The Castles Dr. Creep r${VERSION} Installer (${BUILDDATE})"
	OutFile  ..\packages\DrCreep-${VERSION}.exe

	;Default installation folder
	InstallDir "$PROGRAMFILES\DrCreep"
  
	;Get installation folder from registry if available
	InstallDirRegKey HKCU "Software\DrCreep" ""

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages
  ; Welcome page
  !insertmacro MUI_PAGE_WELCOME

  !insertmacro MUI_PAGE_LICENSE "..\License.txt"
  ;!insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\DrCreep" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Castles of Dr Creep"

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH 

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Copy Files" drcreepInst

  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  File "..\packages\vcredist_x86.exe"
  File "..\License.txt"
  File "..\Readme.txt"
  File "..\run\creep.exe"
  File "..\run\SDL.dll"

  SetOutPath "$INSTDIR\data"
  File "..\run\data\*"

  SetOutPath "$INSTDIR\data\castles"
  File "..\run\data\castles\*"

  SetOutPath "$INSTDIR"
  ;Store installation folder
  WriteRegStr HKCU "Software\creep" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
 	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    	;Create shortcuts
     CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
     CreateShortCut "$SMPROGRAMS\$StartMenuFolder\DrCreep.lnk" "$INSTDIR\creep.exe"
     CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

    	!insertmacro MUI_STARTMENU_WRITE_END

	CreateShortCut "$DESKTOP\DrCreep.lnk" "$INSTDIR\creep.exe"

	MessageBox MB_YESNO|MB_ICONQUESTION "Install Microsoft Visual C++ 2008 Redistributable Package?" IDNO NoRunVC
    Exec "$INSTDIR\vcredist_x86.exe"
  NoRunVC:

SectionEnd 

;--------------------------------
;Descriptions

	;Language strings
	 LangString DESC_creepInst ${LANG_ENGLISH} "Main Program Installation"

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${creepInst} $(DESC_creepInst)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------

Function .onInit
	# the plugins dir is automatically deleted when the installer exits
	InitPluginsDir
	#File /oname=$PLUGINSDIR\splash.bmp "splash.bmp"
	#optional
	#File /oname=$PLUGINSDIR\splash.wav "C:\myprog\sound.wav"

	#splash::show 2000 $PLUGINSDIR\splash

	Pop $0 ; $0 has '1' if the user closed the splash screen early,

FunctionEnd

Function .onInstSuccess

ExecShell open '$INSTDIR\Readme.txt'

MessageBox MB_YESNO|MB_ICONQUESTION "Launch Dr. Creep?" IDNO NoRun
    Exec "$INSTDIR\creep.exe"
  NoRun:
FunctionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

    Delete "$INSTDIR\vcredist_x86.exe"
    Delete "$INSTDIR\License.txt"
    Delete "$INSTDIR\creep.exe"
    Delete "$INSTDIR\Uninstall.exe"
    Delete "$INSTDIR\Readme.txt"
    Delete "$INSTDIR\SDL.dll"
    Delete "$INSTDIR\data\*"
    Delete "$INSTDIR\data\castles\*"

	RMDir "$INSTDIR\data\castles"
	RMDir "$INSTDIR\data"
	RMDir "$INSTDIR"
		
	IfFileExists $INSTDIR 0 no 
		MessageBox MB_YESNO|MB_ICONQUESTION "Remove all files in the drcreep directory? " IDNO no
			
			Delete $INSTDIR\*.*
			RMDir /r $INSTDIR
			
	  IfFileExists $INSTDIR 0 no 		
      MessageBox MB_OK|MB_ICONEXCLAMATION "Note: $INSTDIR could not be removed."
no:
   
  Delete "$DESKTOP\DrCreep.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\DrCreep.lnk" 
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir  "$SMPROGRAMS\$StartMenuFolder"
 
  DeleteRegKey /ifempty HKCU "Software\DrCreep"

SectionEnd
