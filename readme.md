# BioTools plugin for Notepad++

Contains single function: get the reverse-complement of the selection (assuming a DNA string). If the selection contains non-DNA characters, nothing happens.

## Installation
See the [Notepadd++ documentation](https://npp-user-manual.org/docs/plugins/#how-to-install-a-plugin).

# Usage

Three functions are implemented: reverse-complement, translate, find next ORF. These functions only work on well-defined DNA characters (ACTG), if the input contains spaces, "N" etc, nothing happens. The case is ignored (i.e. you can mix "actg" with "ACTG").

## Reverse-complement

The selection is replaced by its reverse-complement.

Example: `AAAGgg` becomes `ccCTTT`.

## Translate

The selection is replaced by its protein translation.

Example: `TAcCGTTAatGT` becomes `YR*C`.

Note: if the selection's length is not a multiple of 3, nothing happens.

## Find next ORF

If there is a selection, it is updated to start with the first Start codon inside the selection, and end with the first following Stop codon. If there is no selection, the sequence considered is the one between the current cursor position and the end of the document. If there is no Stop codon in frame, the cursor is positioned on the next Start codon.

Important note: if the selection (or the text between the current cursor positiion and the end of the document) contains any non-DNA character (including newlines), nothing happens.



## Issues, questions, source code

See [the GitHub page](https://github.com/AlexWeinreb/biotools_npp). Forks and PR welcome.


