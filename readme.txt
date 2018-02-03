Artistic Style plugin for Notepad++
-----------------------------------
An Artistic Style plugin for Notepad++ to Format C, C++, C++/CLI, Objective-C, C#, and Java Source Code.

Thanks to developers of Artistic Style, Scintilla and Notepad++.

NOTE: Artistic Style Version 3.1
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


Brace Style Options:
--------------------
    None style nothing to do.

    Allman style formatting/indenting.
    Broken braces.

    Java style formatting/indenting.
    Attached braces.

    Kernighan & Ritchie style formatting/indenting.
    Linux braces.

    Stroustrup style formatting/indenting.
    Linux braces.

    Whitesmith style formatting/indenting.
    Broken, indented braces.
    Indented class blocks and switch blocks.

    VTK style formatting/indenting.
    Broken, indented braces except for the opening braces.

    Ratliff style formatting/indenting.
    Attached, indented braces.

    GNU style formatting/indenting.
    Broken braces, indented blocks.

    Linux style formatting/indenting.
    Linux braces, minimum conditional indent is one-half indent.

    Horstmann style formatting/indenting.
    Run-in braces, indented switches.

    One True Brace (1TBS) Style formatting/indenting.
    Linux braces, add braces to all conditionals.

    Google style formatting/indenting.
    Attached braces, indented class modifiers.

    Mozilla style formatting/indenting.
    Linux braces, with broken braces for structs and enums,
    and attached braces for namespaces.

    Pico style formatting/indenting.
    Run-in opening braces and attached closing braces.
    Uses keep one line blocks and keep one line statements.

    Lisp style formatting/indenting.
    Attached opening braces and attached closing braces.
    Uses keep one line statements.

Brace Modify Options:
---------------------
    Attach Namespaces
    Attach braces to a namespace statement.

    Attach Classes
    Attach braces to a class statement.

    Attach Inlines
    Attach braces to class inline function definitions.

    Attach Extern C
    Attach braces to an extern "C" statement.

    Attach closing while
    Attach closing while of do-while to the closing brace.

Function Declaration Options:
---------------------
    Break Func Return Type
    Break Func Return Type Decl
    Break the return type from the function name. Options are
    for the function definitions and the function declarations.

Indentation Options:
--------------------
    Indent classes blocks
    Indent 'class' blocks so that the entire block is indented.

    Indent access modifiers
    Indent 'class' access modifiers, 'public:', 'protected:' or
    'private:', one half indent. The rest of the class is not indented.

    Indent switches blocks
    Indent 'switch' blocks, so that the inner 'case XXX:'
    headers are indented in relation to the switch block.

    Indent case blocks
    Indent case blocks from the 'case XXX:' headers.
    Case statements not enclosed in blocks are NOT indented.

    Indent namespace blocks
    Indent the contents of namespace blocks.

    Indent after parens
    Indent, instead of align, continuation lines following lines
    that contain an opening paren '(' or an assignment '='.

    Indent continuation #
    Indent continuation lines an additional # indents.
    The valid values are 0 thru 4 indents.
    The default value is 1 indent.

    Indent goto labels
    Indent labels so that they appear one indent less than
    the current indentation level, rather than being
    flushed completely to the left (which is the default).

    Indent preproc blocks
    Indent preprocessor blocks at brace level 0.
    Without this option the preprocessor block is not indented.

    Indent preproc conditional
    Indent preprocessor conditional statements #if/#else/#endif
    to the same level as the source code.

    Indent preproc define
    Indent multi-line preprocessor #define statements.

    Indent comments at 1st column
    Indent line comments that start in column one.

    Minimum Conditional Indent #
    Indent a minimal # spaces in a continuous conditional
    belonging to a conditional header.
    The valid values are:
    Zero - no minimal indent.
    One  - indent at least one additional indent.
    Two  - indent at least two additional indents.
    One Half - indent at least one-half an additional indent.
    The default value is Two, two additional indents.

    Maximum continuation Indent #
    Indent a maximal # spaces in a continuation line,
    relative to the previous line.
    The valid values are 40 thru 120.
    The default value is 40.

Padding Options:
----------------
    Pad operators
    Insert space padding around operators.

    Pad commas
    Insert space padding after commas.

    Pad parens outside
    Insert space padding around parenthesis on the outside only.

    Pad first paren outside
    Insert space padding around first parenthesis in a series on
    the outside only.

    Pad parens inside
    Insert space padding around parenthesis on the inside only.

    Pad header
    Insert space padding after paren headers (e.g. 'if', 'for'...).

    Unpad parens
    Remove unnecessary space padding around parenthesis. This
    can be used in combination with the 'pad' options above.

    Delete empty lines
    Delete empty lines within a function or method.
    It will NOT delete lines added by the break-blocks options.

    Fill empty line
    Fill empty lines with the white space of their previous lines.

    Align pointer : None, Type, Middle, Name
    Attach a pointer or reference operator (*, &, or ^) to either
    the operator type (left), middle, or operator name (right).
    To align the reference separately use --align-reference.

    Align reference : None, Type, Middle, Name, Same as Pointer
    Attach a reference operator (&) to either
    the operator type (left), middle, or operator name (right).
    If not set, follow pointer alignment.

Formatting Options:
-------------------
    Break blocks
    Insert empty lines around unrelated blocks, labels, classes, ...

    Break closing header blocks
    Break braces before closing headers (e.g. 'else', 'catch', ...).
    Use with Brace Style : java, kr, stroustrup, linux, or 1tbs.

    Break else/if statements
    Break 'else if()' statements into two different lines.

    Break one line headers
    Break one line headers (e.g. 'if', 'while', 'else', ...) from a
    statement residing on the same line.

    Add braces
    Add braces to unbraced one line conditional statements.

    Add one line braces
    Add one line braces to unbraced one line conditional statements.

    Remove braces
    Remove braces from a braced one line conditional statements.

    Keep one line blocks
    Don't break blocks residing completely on one line.

    Keep one line statements
    Don't break lines containing multiple statements into
    multiple single-statement lines.

    Close templates
    Close ending angle brackets on template definitions.

    Strip comment prefix
    Remove the leading '*' prefix on multi-line comments and
    indent the comment text one indent.

    Maximum Code Line Width #
    max-code-length=# will break the line if it exceeds more than
    # characters. The valid values are 50 thru 200.

    Break line after logical
    If the line contains logical conditionals they will be placed
    first on the new line. The option break-after-logical will
    cause the logical conditional to be placed last on the previous line.

Objective-C Options:
--------------------
    Pad method prefix
    Insert space padding after the '-' or '+' Objective-C method prefix.

    Unpad method prefix
    Remove all space padding after the '-' or '+' Objective-C method prefix.

    Pad return type
    Insert space padding after the Objective-C return type.

    Unpad return type
    Remove all space padding after the Objective-C return type.

    Pad param type
    Insert space padding after the Objective-C return type.

    Unpad param type
    Remove all space padding after the Objective-C return type.

    Align method colons
    Align the colons in an Objective-C method definition.

    Pad method colons : No Change, None, All, After, Before
    Add or remove space padding before or after the colons in an
    Objective-C method call.
