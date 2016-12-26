Artistic Style plugin for Notepad++
-----------------------------------
An Artistic Style plugin for Notepad++ to Format C, C++, C++/CLI, Objective-C, C#, and Java Source Code.

Thanks to developers of Artistic Style, Scintilla and Notepad++.

NOTE: Artistic Style Version 2.06 beta
For information on Artistic Style, please see http://sourceforge.net/projects/astyle/
For information on Scintilla, please see http://www.scintilla.org/ or http://sourceforge.net/projects/scintilla/
For information on Notepad++, please see https://notepad-plus-plus.org/ or https://github.com/notepad-plus-plus/notepad-plus-plus


Name:      NppAStyle
Author:    YWX (wxFortranIndent@163.com)
Created:   2015-8-2
Copyright: (c) YWX <wxFortranIndent@163.com>
License:   GNU General Public License, version 3

Send bug reports and questions to YWX <wxFortranIndent@163.com>.

Please notice that often trying to correct the bug yourself is the
quickest way to fix it. Even if you fail to do it, you may
discover valuable information allowing us to fix it while doing
it. We also give much higher priority to bug reports with patches
fixing the problems so this ensures that your report will be
addressed sooner.

To get the source code, please visit https://github.com/ywx/NppAStyle
Get a local copy of the Artistic Style Plugin For Notepad++ repository with this command:
git clone git://github.com/ywx/NppAStyle.git NppAStyle-Code

Download NppAStyle.7z (include NppAStyle.dll) from https://github.com/ywx/NppAStyle/releases

For information on Git，please see http://git-scm.com/ or https://github.com/git/git
For information on Git for Windows，please see http://msysgit.github.io/

How to Use NppAStyle
====================
1, Copy NppAStyle.dll to <Notepad++ installation directory>\plugins
   Reboot Notepad++, NppAStyle menu will appear in Plugins menu.
2, Or Use Notepad++ Menu Command : Settings => Import => Import plugin(s)...
3, Run Menu Command : NppAStyle => Options ... , Before First Run Menu Command : NppAStyle => Format Code


Bracket Style Options:
----------------------
    None style nothing to do.

    Allman style formatting/indenting.
    Broken brackets.

    Java style formatting/indenting.
    Attached brackets.

    Kernighan & Ritchie style formatting/indenting.
    Linux brackets.

    Stroustrup style formatting/indenting.
    Stroustrup brackets.

    Whitesmith style formatting/indenting.
    Broken, indented brackets.
    Indented class blocks and switch blocks.

    VTK style formatting/indenting.
    Broken, indented brackets, except for opening brackets.

    Banner style formatting/indenting.
    Attached, indented brackets.

    GNU style formatting/indenting.
    Broken brackets, indented blocks.

    Linux style formatting/indenting.
    Linux brackets, minimum conditional indent is one-half indent.

    Horstmann style formatting/indenting.
    Run-in brackets, indented switches.

    One True Brace (1TBS) Style formatting/indenting.
    Linux brackets, add brackets to all conditionals.

    Google style formatting/indenting.
    Attached brackets, indented class modifiers.

    Mozilla style formatting/indenting.
    Linux brackets, with broken brackets for structs and enums,
    and attached brackets for namespaces.

    Pico style formatting/indenting.
    Run-in opening brackets and attached closing brackets.
    Uses keep one line blocks and keep one line statements.

    Lisp style formatting/indenting.
    Attached opening brackets and attached closing brackets.
    Uses keep one line statements.

Bracket Modify Options:
-----------------------
    attachNamespaces
    Attach brackets to a namespace statement.

    attachClasses
    Attach brackets to a class statement.

    attachInlines
    Attach brackets to class inline function definitions.

    attachExternC
    Attach brackets to an extern "C" statement.

Indentation Options:
--------------------
    indent classes
    Indent 'class' blocks so that the entire block is indented.

    indent modifiers
    Indent 'class' access modifiers, 'public:', 'protected:' or
    'private:', one half indent. The rest of the class is not indented.

    indent switches
    Indent 'switch' blocks, so that the inner 'case XXX:'
    headers are indented in relation to the switch block.

    indent cases
    Indent case blocks from the 'case XXX:' headers.
    Case statements not enclosed in blocks are NOT indented.

    indent namespaces
    Indent the contents of namespace blocks.

    indent continuation #
    Indent continuation lines an additional # indents.
    The valid values are 0 thru 4 indents.
    The default value is 1 indent.

    indent labels
    Indent labels so that they appear one indent less than
    the current indentation level, rather than being
    flushed completely to the left (which is the default).

    indent preproc block
    Indent preprocessor blocks at bracket level 0.
    Without this option the preprocessor block is not indented.

    indent preproc cond
    Indent preprocessor conditional statements #if/#else/#endif
    to the same level as the source code.

    indent preproc define
    Indent multi-line preprocessor #define statements.

    indent col1-comments
    Indent line comments that start in column one.

    min conditional indent #
    Indent a minimal # spaces in a continuous conditional
    belonging to a conditional header.
    The valid values are:
    Zero - no minimal indent.
    One  - indent at least one additional indent.
    Two  - indent at least two additional indents.
    One Half - indent at least one-half an additional indent.
    The default value is 2, two additional indents.

    max instatement indent #
    Indent a maximal # spaces in a continuous statement,
    relative to the previous line.
    The valid values are 40 thru 120.
    The default value is 40.

Padding Options:
----------------
    break blocks
    Insert empty lines around unrelated blocks, labels, classes, ...

    pad operators
    Insert space padding around operators.

    pad comma
    Insert space padding around commas and semicolons.

    pad parenthesis outside
    Insert space padding around parenthesis on the outside only.

    pad first paren out
    Insert space padding around first parenthesis in a series on
    the outside only.

    pad parenthesis outside
    Insert space padding around parenthesis on the inside only.

    pad header
    Insert space padding after paren headers (e.g. 'if', 'for'...).

    unpad parenthesis
    Remove unnecessary space padding around parenthesis. This
    can be used in combination with the 'pad' options above.

    delete empty lines
    Delete empty lines within a function or method.
    It will NOT delete lines added by the break-blocks options.

    fill empty lines
    Fill empty lines with the white space of their previous lines.

    align pointer : type, middle, name
    Attach a pointer or reference operator (*, &, or ^) to either
    the operator type (left), middle, or operator name (right).
    To align the reference separately use --align-reference.

    align reference : none, type, middle, name, same as pointer
    Attach a reference operator (&) to either
    the operator type (left), middle, or operator name (right).
    If not set, follow pointer alignment.

Formatting Options:
-------------------
    break closing brackets
    Break brackets before closing headers (e.g. 'else', 'catch', ...).
    Use with Bracket Style : java, kr, stroustrup linux, or 1tbs.

    break elseifs
    Break 'else if()' statements into two different lines.

    add brackets
    Add brackets to unbracketed one line conditional statements.

    add one line brackets
    Add one line brackets to unbracketed one line conditional statements.

    remove brackets
    Remove brackets from a bracketed one line conditional statements.

    keep one line blocks
    Don't break blocks residing completely on one line.

    keep one line statements
    Don't break lines containing multiple statements into
    multiple single-statement lines.

    close templates
    Close ending angle brackets on template definitions.

    remove comment prefix
    Remove the leading '*' prefix on multi-line comments and
    indent the comment text one indent.

    break after logical
    If the line contains logical conditionals they will be placed
    first on the new line. The option break-after-logical will
    cause the logical conditional to be placed last on the previous line.

    max-code-length #
    max-code-length=# will break the line if it exceeds more than
    # characters. The valid values are 50 thru 200.
    If the line contains logical conditionals they will be placed
    first on the new line. The option break-after-logical will
    cause the logical conditional to be placed last on the previous line.

Objective-C Options:
--------------------
    pad method prefix
    Insert space padding after the '-' or '+' Objective-C method prefix.

    unpad method prefix
    Remove all space padding after the '-' or '+' Objective-C method prefix.

    pad return type
    Insert space padding after the Objective-C return type.

    unpad return type
    Remove all space padding after the Objective-C return type.

    pad param type
    Insert space padding after the Objective-C return type.

    unpad param type
    Remove all space padding after the Objective-C return type.

    align method colon
    Align the colons in an Objective-C method definition.

    pad method colon : no change, none, all, after, before
    Add or remove space padding before or after the colons in an
    Objective-C method call.
