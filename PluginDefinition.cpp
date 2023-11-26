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
    setCommand(1, TEXT("Translate"), translate, NULL, false);
    setCommand(2, TEXT("Next ORF"), nextORF, NULL, false);
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
            if (c != 'a' && c != 'c' && c != 't' && c != 'g' && c != ' ')
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

std::string translateDNA(std::string seq)
{

    std::transform(seq.begin(), seq.end(), seq.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

    std::string translation;
    std::string codon;
    for (int i = 0; i < seq.length() / 3; ++i)
    {
        
        codon = seq.substr(i*3, 3);

        /* Code written with R:
        * Biostrings::GENETIC_CODE |>
              purrr::imap(~paste0('if(codon == "',.y, '"){
                                  translation += "',.x,'";
                                }')) |>
              paste0(collapse = "\n") |>
              clipr::write_clip()
        */

        if (codon == "TTT") {
            translation += "F";
        }
        if (codon == "TTC") {
            translation += "F";
        }
        if (codon == "TTA") {
            translation += "L";
        }
        if (codon == "TTG") {
            translation += "L";
        }
        if (codon == "TCT") {
            translation += "S";
        }
        if (codon == "TCC") {
            translation += "S";
        }
        if (codon == "TCA") {
            translation += "S";
        }
        if (codon == "TCG") {
            translation += "S";
        }
        if (codon == "TAT") {
            translation += "Y";
        }
        if (codon == "TAC") {
            translation += "Y";
        }
        if (codon == "TAA") {
            translation += "*";
        }
        if (codon == "TAG") {
            translation += "*";
        }
        if (codon == "TGT") {
            translation += "C";
        }
        if (codon == "TGC") {
            translation += "C";
        }
        if (codon == "TGA") {
            translation += "*";
        }
        if (codon == "TGG") {
            translation += "W";
        }
        if (codon == "CTT") {
            translation += "L";
        }
        if (codon == "CTC") {
            translation += "L";
        }
        if (codon == "CTA") {
            translation += "L";
        }
        if (codon == "CTG") {
            translation += "L";
        }
        if (codon == "CCT") {
            translation += "P";
        }
        if (codon == "CCC") {
            translation += "P";
        }
        if (codon == "CCA") {
            translation += "P";
        }
        if (codon == "CCG") {
            translation += "P";
        }
        if (codon == "CAT") {
            translation += "H";
        }
        if (codon == "CAC") {
            translation += "H";
        }
        if (codon == "CAA") {
            translation += "Q";
        }
        if (codon == "CAG") {
            translation += "Q";
        }
        if (codon == "CGT") {
            translation += "R";
        }
        if (codon == "CGC") {
            translation += "R";
        }
        if (codon == "CGA") {
            translation += "R";
        }
        if (codon == "CGG") {
            translation += "R";
        }
        if (codon == "ATT") {
            translation += "I";
        }
        if (codon == "ATC") {
            translation += "I";
        }
        if (codon == "ATA") {
            translation += "I";
        }
        if (codon == "ATG") {
            translation += "M";
        }
        if (codon == "ACT") {
            translation += "T";
        }
        if (codon == "ACC") {
            translation += "T";
        }
        if (codon == "ACA") {
            translation += "T";
        }
        if (codon == "ACG") {
            translation += "T";
        }
        if (codon == "AAT") {
            translation += "N";
        }
        if (codon == "AAC") {
            translation += "N";
        }
        if (codon == "AAA") {
            translation += "K";
        }
        if (codon == "AAG") {
            translation += "K";
        }
        if (codon == "AGT") {
            translation += "S";
        }
        if (codon == "AGC") {
            translation += "S";
        }
        if (codon == "AGA") {
            translation += "R";
        }
        if (codon == "AGG") {
            translation += "R";
        }
        if (codon == "GTT") {
            translation += "V";
        }
        if (codon == "GTC") {
            translation += "V";
        }
        if (codon == "GTA") {
            translation += "V";
        }
        if (codon == "GTG") {
            translation += "V";
        }
        if (codon == "GCT") {
            translation += "A";
        }
        if (codon == "GCC") {
            translation += "A";
        }
        if (codon == "GCA") {
            translation += "A";
        }
        if (codon == "GCG") {
            translation += "A";
        }
        if (codon == "GAT") {
            translation += "D";
        }
        if (codon == "GAC") {
            translation += "D";
        }
        if (codon == "GAA") {
            translation += "E";
        }
        if (codon == "GAG") {
            translation += "E";
        }
        if (codon == "GGT") {
            translation += "G";
        }
        if (codon == "GGC") {
            translation += "G";
        }
        if (codon == "GGA") {
            translation += "G";
        }
        if (codon == "GGG") {
            translation += "G";
        }
    }
    return translation;
}

void translate()
{
    // Get a handle to the current Scintilla editor
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;

    HWND scintillaHandle = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;


    std::string selection = getSelectedText(scintillaHandle);


    bool check = checkCharacters(selection, "DNA");

    if (!check)
    {
        /*std::wstring debugMessage2 = L"Characters were not all recognized, aborting";
        OutputDebugString(debugMessage2.c_str());*/
        return;
    }
    if (selection.length() % 3 != 0)
    {
        /*std::wstring debugMessage2 = L"Selection not a multiple of 3, aborting";
        OutputDebugString(debugMessage2.c_str());*/
        return;
    }

    std::string translation = translateDNA(selection);
    replaceSelection(translation, scintillaHandle);
}

char* getText(HWND sciHandle, int start_pos, int end_pos)
{
    Sci_TextRangeFull tr;
    tr.chrg.cpMin = start_pos;
    tr.chrg.cpMax = end_pos;
    tr.lpstrText = new char[end_pos - start_pos + 1];

    ::SendMessage(sciHandle, SCI_GETTEXTRANGEFULL, 0, (LPARAM)&tr);

    return(tr.lpstrText);
}

size_t nextStartCodon(std::string text)
{
    // find the position of the first start codon
    size_t pos = text.find("ATG");
    return(pos);
}

size_t nextStopCodon(std::string text, size_t pos_start)
{
    // find position of next stop codon
    text = text.substr(pos_start, text.length());
    std::string translation = translateDNA(text);

    size_t pos = translation.find("*");
    return( pos_start + 3*(pos+1) );
}


void nextORF(){

    // Get a handle to the current Scintilla editor
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;

    HWND scintillaHandle = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;


    /* Two cases
    * if text is selected, look for ORF inside it
    * if no text selected, look for ORF between current cursor position and end of document
    */


    int selStart = (int)::SendMessage(scintillaHandle, SCI_GETSELECTIONSTART, 0, 0);
    int selEnd = (int)::SendMessage(scintillaHandle, SCI_GETSELECTIONEND, 0, 0);

    if (selEnd == selStart)
    {
        selEnd = (int)::SendMessage(scintillaHandle, SCI_GETTEXTLENGTH, 0, 0);
    }

    std::string selText = getText(scintillaHandle, selStart, selEnd);
    

    bool check = checkCharacters(selText, "DNA");

    if (!check)
    {
        std::wstring debugMessage2 = L"Characters were not all recognized, aborting";
        OutputDebugString(debugMessage2.c_str());
        return;
    }
    
    
    size_t posStart = nextStartCodon(selText);
    size_t posStop = nextStopCodon(selText, posStart);

    // back to document coords
    posStart = posStart + selStart;
    posStop = posStop + selStart;

    if (posStop == selStart)
    {
        posStop = selEnd;
    }


    ::SendMessage(scintillaHandle, SCI_SETSEL, posStart, posStop);


    
    
    /*std::string posstr = " --- start: " + std::to_string(posStart) +
                         " -- end: "+std::to_string(posStop)+" --- ";
    replaceSelection(posstr, scintillaHandle);
    */


}