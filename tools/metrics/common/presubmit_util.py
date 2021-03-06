# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os
import sys
import logging
import shutil

import diff_util

sys.path.insert(1, os.path.join(sys.path[0], '..', '..', 'python'))
from google import path_utils

def DoPresubmitMain(argv, original_filename, backup_filename, script_name,
                    prettyFn):
  """Execute presubmit/pretty printing for the target file.

  Args:
    argv: command line arguments
    original_filename: The filename to read from.
    backup_filename: When pretty printing, move the old file contents here.
    script_name: The name of the script to run for pretty printing.
    prettyFn: A function which takes the original xml content and produces
        pretty printed xml.

  Returns:
    An exit status.  Non-zero indicates errors.
  """
  logging.basicConfig(level=logging.INFO)
  presubmit = ('--presubmit' in argv)

  # If there is a description xml in the current working directory, use that.
  # Otherwise, use the one residing in the same directory as this script.
  xml_dir = os.getcwd()
  if not os.path.isfile(os.path.join(xml_dir, original_filename)):
    xml_dir = path_utils.ScriptDir()

  xml_path = os.path.join(xml_dir, original_filename)

  # Save the original file content.
  logging.info('Loading %s...', os.path.relpath(xml_path))
  with open(xml_path, 'rb') as f:
    original_xml = f.read()

  # Check there are no CR ('\r') characters in the file.
  if '\r' in original_xml:
    logging.error('DOS-style line endings (CR characters) detected - these are '
                  'not allowed. Please run dos2unix %s', original_filename)
    sys.exit(1)

  try:
    pretty = prettyFn(original_xml)
  except Error:
    logging.error('Aborting parsing due to fatal errors.')
    sys.exit(1)

  if original_xml == pretty:
    logging.info('%s is correctly pretty-printed.', original_filename)
    sys.exit(0)
  if presubmit:
    logging.error('%s is not formatted correctly; run %s to fix.',
                  original_filename, script_name)
    sys.exit(1)

  # Prompt user to consent on the change.
  if not diff_util.PromptUserToAcceptDiff(
      original_xml, pretty, 'Is the new version acceptable?'):
    logging.error('Diff not accepted. Aborting.')
    sys.exit(1)

  logging.info('Creating backup file: %s', backup_filename)
  shutil.move(xml_path, os.path.join(xml_dir, backup_filename))

  with open(xml_path, 'wb') as f:
    f.write(pretty)
  logging.info('Updated %s. Don\'t forget to add it to your changelist',
               xml_path)
  sys.exit(0)
