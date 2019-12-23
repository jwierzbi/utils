"""Style strings with ANSI escape sequences.

To apply styles imply use:

    Style.style(string, bold=True, color=Style.Color.RED)
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
        BLACK = '30'
        RED = '31'
        GREEN = '32'
        YELLOW = '33'
        BLUE = '34'
        MAGENTA = '35'
        CYAN = '36'
        LIGHT_GRAY = '37'
        DARK_GRAY = '90'
        LIGHT_RED = '91'
        LIGHT_GREEN = '92'
        LIGHT_YELLOW = '93'
        LIGHT_BLUE = '94'
        LIGHT_MAGENTA = '95'
        LIGHT_CYAN = '96'
        WHITE = '97'

    @staticmethod
    def style(string, bold=False, italic=False, underline=False,
              color: Color=None) -> str:
        """Apply style to the string.

        Args:
            bold:      if True the text will be printed as bold
            italic:    if True the text will be printed as italic
            underline: if True the text will be underlined
            color:     if specified the text will be colored

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
        if color:
            start_seq.append(color.value)
            end_seq.append('39')

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
            start_cmds = ('1', '3', '4', *(x.value for x in Style.Color))
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
