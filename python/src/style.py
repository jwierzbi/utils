"""Style strings with ANSI escape sequences.

To apply styles imply use:

    Style.style(string, bold=True, fg=Style.Color.RED)
"""

"""
Copyright (c) 2019 Jarosław Wierzbicki

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

import re
from enum import Enum

class Style:
    """Style string with ANSI escape sequences.

    The styles are applied at the beginning of the string and reverted
    at the end of it. It's possible to nest style. The outer styles will
    be then reapplied when inner style ends."""
    class Color(Enum):
        """Color definitions."""

        def __new__(cls, fg, bg):
            obj = object.__new__(cls)
            obj._value_ = {'fg': fg, 'bg': bg}
            return obj

        BLACK = ('30', '40')
        RED = ('31', '41')
        GREEN = ('32', '42')
        YELLOW = ('33', '43')
        BLUE = ('34', '44')
        MAGENTA = ('35', '45')
        CYAN = ('36', '46')
        LIGHT_GRAY = ('37', '47')
        DARK_GRAY = ('90', '100')
        LIGHT_RED = ('91', '101')
        LIGHT_GREEN = ('92', '102')
        LIGHT_YELLOW = ('93', '103')
        LIGHT_BLUE = ('94', '104')
        LIGHT_MAGENTA = ('95', '105')
        LIGHT_CYAN = ('96', '106')
        WHITE = ('97', '107')

    @staticmethod
    def style(string, bold=False, italic=False, underline=False,
              fg: Color=None, bg: Color=None) -> str:
        """Apply style to the string.

        Args:
            bold:      if True the text will be printed as bold
            italic:    if True the text will be printed as italic
            underline: if True the text will be underlined
            fg:        if specified the text will be colored
            bg:        if specified the background of the text will be colored

        Returns:
            String with applied styles of original string if no styles
            were specified.
        """
        start_seq = []
        end_seq = []

        # create set style and reset style sequences
        if bold:
            start_seq.append('1')
            end_seq.append('22')
        if italic:
            start_seq.append('3')
            end_seq.append('23')
        if underline:
            start_seq.append('4')
            end_seq.append('24')
        if fg:
            start_seq.append(fg.value['fg'])
            end_seq.append('39')
        if bg:
            start_seq.append(bg.value['bg'])
            end_seq.append('49')

        # create the ANSI escape sequences
        start_seq = '\x1b[{}m'.format(';'.join(start_seq)) if start_seq else ''
        end_seq = '\x1b[{}m'.format(';'.join(end_seq)) if end_seq else ''

        # start the string
        out = start_seq

        # find all of the already applied styles
        offset = 0
        for match in re.finditer(r'\x1b\[([0-9;]*)m', string):
            # added the found sequence
            out += string[offset:match.start()]
            # determine if the nested sequence is a start or end sequence
            start_cmds = (
                '1', '3', '4',
                *(x.value['fg'] for x in Style.Color),
                *(x.value['bg'] for x in Style.Color),
            )
            if match.group(1).split(';')[0] in start_cmds:
                # this is a start sequence so just add it
                out += string[match.start():match.end()]
            else:
                # this is an end sequence so add it and append start sequence
                # to re-enable previous styles (those styles will stack up)
                out += string[match.start():match.end()] + start_seq
            # set the next offset
            offset = match.end()

        # append the remaining part of the string and the end sequence
        out += string[offset:] + end_seq

        return out
