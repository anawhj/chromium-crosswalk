# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
import sys

from telemetry.core import wpr_modes
from telemetry import decorators
from telemetry.page import page
from telemetry.unittest_util import options_for_unittests
from telemetry.unittest_util import page_test_test_case

from measurements import smoothness


class FakeTracingController(object):
  def __init__(self):
    self.category_filter = None
  def Start(self, options, category_filter):
    del options  # unused
    self.category_filter = category_filter

  def IsChromeTracingSupported(self):
    return True


class FakePlatform(object):
  def __init__(self):
    self.tracing_controller = FakeTracingController()


class FakeBrowser(object):
  def __init__(self):
    self.platform = FakePlatform()


class FakeTab(object):
  def __init__(self):
    self.browser = FakeBrowser()

  def ExecuteJavaScript(self, js):
    pass

class SmoothnessUnitTest(page_test_test_case.PageTestTestCase):
  """Smoke test for smoothness measurement

     Runs smoothness measurement on a simple page and verifies
     that all metrics were added to the results. The test is purely functional,
     i.e. it only checks if the metrics are present and non-zero.
  """
  def testSyntheticDelayConfiguration(self):
    test_page = page.Page('http://dummy', None)
    test_page.synthetic_delays = {
        'cc.BeginMainFrame': { 'target_duration': 0.012 },
        'cc.DrawAndSwap': { 'target_duration': 0.012, 'mode': 'alternating' },
        'gpu.PresentingFrame': { 'target_duration': 0.012 }
    }

    tab = FakeTab()
    measurement = smoothness.Smoothness()
    measurement.WillStartBrowser(tab.browser.platform)
    measurement.WillNavigateToPage(test_page, tab)

    expected_synthetic_delay = set([
        'DELAY(cc.BeginMainFrame;0.012000;static)',
        'DELAY(cc.DrawAndSwap;0.012000;alternating)',
        'DELAY(gpu.PresentingFrame;0.012000;static)',
    ])
    tracing_controller = tab.browser.platform.tracing_controller
    actual_synthetic_delay = (
      tracing_controller.category_filter.synthetic_delays)

    if expected_synthetic_delay != actual_synthetic_delay:
      sys.stderr.write("Expected category filter: %s\n" %
                       repr(expected_synthetic_delay))
      sys.stderr.write("Actual category filter filter: %s\n" %
                       repr(actual_synthetic_delay))
    self.assertEquals(expected_synthetic_delay, actual_synthetic_delay)

  def setUp(self):
    self._options = options_for_unittests.GetCopy()
    self._options.browser_options.wpr_mode = wpr_modes.WPR_OFF

  @decorators.Disabled('chromeos')  # crbug.com/483212
  def testSmoothness(self):
    ps = self.CreatePageSetFromFileInUnittestDataDir('scrollable_page.html')
    measurement = smoothness.Smoothness()
    results = self.RunMeasurement(measurement, ps, options=self._options)
    self.assertEquals(0, len(results.failures))

    frame_times = results.FindAllPageSpecificValuesNamed('frame_times')
    self.assertEquals(len(frame_times), 1)
    self.assertGreater(frame_times[0].GetRepresentativeNumber(), 0)

    mean_frame_time = results.FindAllPageSpecificValuesNamed('mean_frame_time')
    self.assertEquals(len(mean_frame_time), 1)
    self.assertGreater(mean_frame_time[0].GetRepresentativeNumber(), 0)

    frame_time_discrepancy = results.FindAllPageSpecificValuesNamed(
        'frame_time_discrepancy')
    self.assertEquals(len(frame_time_discrepancy), 1)
    self.assertGreater(frame_time_discrepancy[0].GetRepresentativeNumber(), 0)

    percentage_smooth = results.FindAllPageSpecificValuesNamed(
        'percentage_smooth')
    self.assertEquals(len(percentage_smooth), 1)
    self.assertGreaterEqual(percentage_smooth[0].GetRepresentativeNumber(), 0)

    mean_input_event_latency = results.FindAllPageSpecificValuesNamed(
        'mean_input_event_latency')
    if mean_input_event_latency:
      self.assertEquals(len(mean_input_event_latency), 1)
      self.assertGreater(
          mean_input_event_latency[0].GetRepresentativeNumber(), 0)

  @decorators.Enabled('android')  # SurfaceFlinger is android-only
  def testSmoothnessSurfaceFlingerMetricsCalculated(self):
    ps = self.CreatePageSetFromFileInUnittestDataDir('scrollable_page.html')
    measurement = smoothness.Smoothness()
    results = self.RunMeasurement(measurement, ps, options=self._options)
    self.assertEquals(0, len(results.failures))

    avg_surface_fps = results.FindAllPageSpecificValuesNamed('avg_surface_fps')
    self.assertEquals(1, len(avg_surface_fps))
    self.assertGreater(avg_surface_fps[0].GetRepresentativeNumber, 0)

    jank_count = results.FindAllPageSpecificValuesNamed('jank_count')
    self.assertEquals(1, len(jank_count))
    self.assertGreater(jank_count[0].GetRepresentativeNumber(), -1)

    max_frame_delay = results.FindAllPageSpecificValuesNamed('max_frame_delay')
    self.assertEquals(1, len(max_frame_delay))
    self.assertGreater(max_frame_delay[0].GetRepresentativeNumber, 0)

    frame_lengths = results.FindAllPageSpecificValuesNamed('frame_lengths')
    self.assertEquals(1, len(frame_lengths))
    self.assertGreater(frame_lengths[0].GetRepresentativeNumber, 0)

  def testCleanUpTrace(self):
    self.TestTracingCleanedUp(smoothness.Smoothness, self._options)
