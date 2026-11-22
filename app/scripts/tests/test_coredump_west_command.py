# SPDX-License-Identifier: Apache-2.0

import unittest
from collections import deque
from contextlib import redirect_stdout
from io import StringIO
from unittest.mock import patch

from coredump_west_command import CoredumpWestCommand


class CoredumpStreamTest(unittest.TestCase):
    begin = "#CD:BEGIN#"
    body = "\n[00:00:00] <err> coredump: #CD:abcd\n"
    end = "#CD:END#"

    def capture(self, *chunks):
        pending = deque(chunks)
        with redirect_stdout(StringIO()), patch("coredump_west_command.time.sleep"):
            return CoredumpWestCommand()._parse_coredump_stream(pending.popleft, lambda: bool(pending))

    def test_markers_can_split_at_any_position(self):
        for begin_split in range(1, len(self.begin)):
            for end_split in range(1, len(self.end)):
                with self.subTest(begin_split=begin_split, end_split=end_split):
                    result = self.capture(
                        "unrelated log " + self.begin[:begin_split],
                        None,
                        self.begin[begin_split:] + self.body + self.end[:end_split],
                        self.end[end_split:] + "\ntrailing log",
                    )
                    self.assertEqual(result, self.begin + self.body + self.end)

    def test_complete_dump_in_one_read(self):
        dump = self.begin + self.body + self.end
        self.assertEqual(self.capture("log " + dump + "\nmore logs"), dump)

    def test_ignores_end_marker_from_an_earlier_dump(self):
        dump = self.begin + self.body + self.end
        self.assertEqual(self.capture(self.end + "\n" + dump), dump)

    def test_resyncs_to_new_dump_after_an_incomplete_one(self):
        dump = self.begin + self.body + self.end
        self.assertEqual(self.capture(self.begin + "\npartial\n", dump), dump)

    def test_stream_without_begin_marker_returns_empty(self):
        self.assertEqual(self.capture("unrelated log", self.end, "more logs"), "")


if __name__ == "__main__":
    unittest.main()
