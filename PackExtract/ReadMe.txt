================================================== ==============================
              MFC library: PackExtract project overview
================================================== ==============================

PackExtract application using the application wizard creates. This application
The program describes how to create a basic MFC application use bapbeop

PackExtract application configuration file for each file contains an overview
Can.

PackExtract.vcproj
    Generated using an Application Wizard VC + + project is the main project file.
    Platform file Visual C + + that generated the file version information and an application selected in the wizard,
    Configuration, information such as the project functions are included.

PackExtract.h
    Of the application is the main header file. Resource.h involved in other projects, such as the file
    CPackExtractApp application specific header and the class is declared.

PackExtract.cpp
    Of CPackExtractApp application class that contains the main application source file.

PackExtract.rc
    All of the Microsoft Windows resources that the program uses the files listed.
    Files are stored in the RES subdirectory icons, bitmaps, and cursors that
    In Microsoft Visual C + + You can also edit this file directly. Users of the project
    Resources in 1033.

res \ PackExtract.ico
    Is an icon file, which is used as the application's icon.
    This icon is included by the main resource file, PackExtract.rc

res \ PackExtract.rc2
    This is a file that contains a resource that can not be edited in the Microsoft Visual C + +.
    Includes all resources that can not be edited in the resource editor in this file.

/ / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

Wizard application in a single dialog box, create a class.
PackExtractDlg.h, PackExtractDlg.cpp - the dialog box
    This is the file that contains the CPackExtractDlg class. This class of application
    Behavior is defined for the main dialog box. In the dialog box template PackExtract.rc
    And Microsoft Visual C + + and can be edited
/ / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

Other standard files

StdAfx.h, StdAfx.cpp
    Precompiled header files (PCH) PackExtract.pch and precompiled
    File is used when building the format file StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource ID.
    Microsoft Visual C + + reads and updates this file.

/ / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

Other Information

Application Wizard uses "TODO:" should add to or customize to
Indicates that the source code.

Use MFC in a shared DLL from the application, and the language of the application of the operating system
Is different from the language in the Microsoft Visual C + + CD-ROM Win \ System directory
The region's resources, MFC70XXX.DLL computer system or system32 directory
Copy and then rename the MFCLOC.DLL. "XXX" representing the language
Abbreviation. For example, the German translations MFC70DEU.DLL the resource are included.
If this work is not part of the application UI elements in the language of the operating system
Will remain.

/ / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /