"""Tests for gbench.util."""

import json
import os
import tempfile
import unittest

from gbench import util


class TestLoadBenchmarkResults(unittest.TestCase):
    def setUp(self):
        # A name whose UTF-8 bytes do not all decode in Windows code pages.
        self.name = "BM_\u0401\u0436\u0438\u043a/8"
        results = {
            "context": {},
            "benchmarks": [
                {
                    "name": self.name,
                    "run_name": self.name,
                    "run_type": "iteration",
                    "iterations": 1,
                    "real_time": 1.0,
                    "cpu_time": 1.0,
                    "time_unit": "ns",
                }
            ],
        }
        fd, self.path = tempfile.mkstemp(suffix=".json")
        with os.fdopen(fd, "w", encoding="utf-8") as f:
            json.dump(results, f, ensure_ascii=False)

    def tearDown(self):
        os.unlink(self.path)

    def test_non_ascii_names_are_read_as_utf8(self):
        self.assertEqual(util.classify_input_file(self.path)[0], util.IT_JSON)
        results = util.load_benchmark_results(self.path, None)
        self.assertEqual(results["benchmarks"][0]["name"], self.name)


if __name__ == "__main__":
    unittest.main()
