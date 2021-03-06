# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.


from __future__ import print_function
from unittest import TestCase

from pyqrllib import pyqrllib


class TestMisc(TestCase):
    def __init__(self, *args, **kwargs):
        super(TestMisc, self).__init__(*args, **kwargs)

    def test_data_to_hex1(self):
        hexstring = pyqrllib.bin2hstr(b'\x00\x11\x22\x33', 0)
        self.assertEqual(hexstring, '00112233')

    def test_data_to_hex2(self):
        hexstring = pyqrllib.bin2hstr('test', 0)
        self.assertEqual(hexstring, '74657374')

    def test_exception(self):
        i = 0
        try:
            x = pyqrllib.hstr2bin('Z')
        except Exception as e:
            i = 1
        self.assertEqual(i, 1)

        i = 0
        try:
            x = pyqrllib.hstr2bin('Z0')
        except Exception as e:
            i = 1
        self.assertEqual(i, 1)
