//this file is part of notepad++
//Copyright (C)2022 Don HO <don.h@free.fr>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <string>
#include <algorithm>

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE /*hModule*/)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(0, TEXT("Reverse-Complement"), revcomp, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//

std::string getSelectedText(HWND scintillaHandle)
{
    std::string selectedText;

    

    // Determine the buffer size needed for the selected text
    LPARAM length = ::SendMessage(scintillaHandle, SCI_GETSELTEXT, 0, (LPARAM)nullptr);

    if (length > 0)
    {
        char* buffer = new char[length + 1];
        buffer[0] = '\0'; // Ensure the buffer is null-terminated

        // Retrieve the selected text
        ::SendMessage(scintillaHandle, SCI_GETSELTEXT, 0, (LPARAM)buffer);
        selectedText = buffer;
        delete[] buffer;
    }
    else {
        selectedText = "";
    }

    return selectedText;
}

void replaceSelection(std::string text, HWND scintillaHandle)
{
    /*std::wstring debugMessage = L"Will replace current selection with: " + std::wstring(text.begin(), text.end());
    OutputDebugString(debugMessage.c_str());*/

    ::SendMessage(scintillaHandle, SCI_REPLACESEL, 0, (LPARAM)text.c_str());
}


bool checkCharacters(std::string seq, std::string charset)
{
    if (charset == "DNA")
    {
        std::transform(seq.begin(), seq.end(), seq.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        for (char c: seq)
        {
            if (c != 'a' && c != 'c' && c != 't' && c != 'g')
            {
                /*std::string cs(1, c);
                std::wstring debugMessage = L"Incorrect character: " + std::wstring(cs.begin(), cs.end());
                OutputDebugString(debugMessage.c_str());*/
                return false;
            }
        }
    }
    else
    {
        // undefined charset
        /*std::wstring debugMessage2 = L"Undefined charset: " + std::wstring(charset.begin(), charset.end());
        OutputDebugString(debugMessage2.c_str());*/
        return false;
    }
    return true;
}

std::string reverseComplement(std::string seq)
{
    std::reverse(seq.begin(), seq.end());


    std::string comp;
    char c;
    for (int i = 0; i < seq.length(); ++i)
    {
        switch (seq[i])
        {
        case 'a':
            c = 't';
            break;
        case 'A':
            c = 'T';
            break;
        case 't':
            c = 'a';
            break;
        case 'T':
            c = 'A';
            break;
        case 'c':
            c = 'g';
            break;
        case 'C':
            c = 'G';
            break;
        case 'g':
            c = 'c';
            break;
        case 'G':
            c = 'C';
            break;
        default:
            c = 'N';
        }
        comp += c;
    }

    /*std::wstring debugMessage = L"Finished rev-comp: " + std::wstring(comp.begin(), comp.end());
    OutputDebugString(debugMessage.c_str());*/

    return comp;
}

void revcomp()
{

    // Get a handle to the current Scintilla editor
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return ;

    HWND scintillaHandle = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;


    std::string selection = getSelectedText(scintillaHandle);
    

    bool check = checkCharacters(selection, "DNA");

    if (!check)
    {
        /*std::wstring debugMessage2 = L"Characters were not all recognized, aborting";
        OutputDebugString(debugMessage2.c_str());*/
        return;
    }
    std::string revcomp = reverseComplement(selection);
    replaceSelection(revcomp, scintillaHandle);
}


