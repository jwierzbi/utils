#!/usr/bin/env python3

import sys
import unittest
from src.style import Style

class StyleTest(unittest.TestCase):
    def test__no_style(self):
        text = Style.style(
            'Lorem ipsum dolor sit amet, consectetur adipiscing elit.'
        )
        self.assertEqual(
            'Lorem ipsum dolor sit amet, consectetur adipiscing elit.',
            text
        )
    
    def test__simple_bold_style(self):
        text = Style.style('Lorem ipsum...', bold=True)
        self.assertEqual('\x1b[1mLorem ipsum...\x1b[22m', text)
    
    def test__simple_italic_style(self):
        text = Style.style('Lorem ipsum...', italic=True)
        self.assertEqual('\x1b[3mLorem ipsum...\x1b[23m', text)
    
    def test__simple_underline_style(self):
        text = Style.style('Lorem ipsum...', underline=True)
        self.assertEqual('\x1b[4mLorem ipsum...\x1b[24m', text)
    
    def test__simple_color_style(self):
        text = Style.style('Lorem ipsum...', color=Style.Color.RED)
        self.assertEqual('\x1b[31mLorem ipsum...\x1b[39m', text)

    def test__simple_bold_and_italic_style(self):
        text = Style.style('Lorem ipsum...', bold=True, italic=True)
        self.assertEqual('\x1b[1;3mLorem ipsum...\x1b[22;23m', text)

    def test__simple_underline_and_color_style(self):
        text = Style.style('Lorem ipsum...', underline=True,
                           color=Style.Color.GREEN)
        self.assertEqual('\x1b[4;32mLorem ipsum...\x1b[24;39m', text)

    def test__double_nested_style(self):
        text = Style.style(
            Style.style('Lorem', color=Style.Color.LIGHT_RED) +
            ' ipsum ' +
            Style.style('dolor', bold=True, underline=True) +
            ' sit amet, consectetur adipiscing ' +
            Style.style('elit.', underline=True, bold=True),
            italic=True, bold=True, color=Style.Color.YELLOW
        )
        self.assertEqual(
            '\x1b[1;3;33m\x1b[91mLorem\x1b[39m\x1b[1;3;33m ipsum '
            '\x1b[1;4mdolor\x1b[22;24m\x1b[1;3;33m sit amet, consectetur '
            'adipiscing \x1b[1;4melit.\x1b[22;24m\x1b[1;3;33m\x1b[22;23;39m',
            text
        )

    def test__triple_nested_style(self):
        text = Style.style(
            Style.style(
                'Lorem ipsum ' +
                Style.style('dolor', bold=True, underline=True) +
                ' sit amet,',
                color=Style.Color.LIGHT_RED) +
            ' consectetur adipiscing elit.',
            italic=True, bold=True, color=Style.Color.YELLOW
        )
        self.assertEqual(
            '\x1b[1;3;33m\x1b[91mLorem ipsum \x1b[1;4mdolor\x1b[22;24m'
            '\x1b[1;3;33m\x1b[91m sit amet,\x1b[39m\x1b[1;3;33m '
            'consectetur adipiscing elit.\x1b[22;23;39m',
            text
        )

    def test__quadruple_nested_style(self):
        text = Style.style(
            Style.style(
                'Lorem ipsum ' +
                Style.style(
                    'dolor' + Style.style(
                        ' consectetur ',
                    color=Style.Color.MAGENTA) + 'adipiscing',
                underline=True, color=Style.Color.GREEN) + ' elit',
            italic=True, color=Style.Color.YELLOW) + '. The ',
        bold=True, color=Style.Color.RED) + 'end.'

        self.assertEqual(
            '\x1b[1;31m\x1b[3;33mLorem ipsum \x1b[4;32mdolor'
            '\x1b[35m consectetur \x1b[39m\x1b[1;31m\x1b[3;33m\x1b[4;32m'
            'adipiscing\x1b[24;39m\x1b[1;31m\x1b[3;33m elit\x1b[23;39m'
            '\x1b[1;31m. The \x1b[22;39mend.',
            text
        )

if __name__ == '__main__':
    unittest.main()