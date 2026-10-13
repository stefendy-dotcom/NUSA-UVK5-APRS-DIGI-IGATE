"""Prevent GPS release filenames from being counted as iGATE/DIGI or source."""
import unittest
from update_download_counts import classify_asset, badge_payload


class DownloadCounterTests(unittest.TestCase):
    def test_tracker_packed_bin(self):
        self.assertEqual(
            classify_asset("NUSA_UVK5_GPS_APRS_TRACKER_REV1D_PTT_GUARD_PRETEST.packed.bin"),
            "gps-tracker")

    def test_tracker_source_zip_not_counted(self):
        self.assertIsNone(classify_asset(
            "NUSA_UVK5_GPS_APRS_TRACKER_REV1D_PTT_GUARD_PRETEST_SOURCE.zip"))

    def test_legacy_categories_unchanged(self):
        cases = {
            "NUSA_UVK5_DIGI_REV1AB.packed.bin": "digi-normal",
            "NUSA_UVK5_DIGI_REV1AB_STANDALONE.packed.bin": "digi-standalone",
            "NUSA_UVK5_IGATE_REV1AB.packed.bin": "igate-normal",
            "NUSA_UVK5_IGATE_REV1AB_STANDALONE.packed.bin": "igate-standalone",
            "NUSA_ESP32_IGATE_REV1C.zip": "esp32-igate",
            "NUSA_ESP32-C3_IGATE_REV1C.zip": "esp32c3-igate",
        }
        for filename, expected in cases.items():
            with self.subTest(filename=filename):
                self.assertEqual(classify_asset(filename), expected)

    def test_ignored(self):
        for filename in ("README.md", "SHA256SUMS.txt", "gps-tracker-setup.zip"):
            with self.subTest(filename=filename):
                self.assertIsNone(classify_asset(filename))

    def test_tracker_badge_zero(self):
        self.assertEqual(badge_payload("GPS Tracker", 0, "blue")["message"], "0 downloads")


if __name__ == "__main__":
    unittest.main()
