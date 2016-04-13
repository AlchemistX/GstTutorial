/* GStreamer unit tests for the fakeadec
 *
 * Copyright 2016 LGE Corporation.
 *  @author: Hoonhee Lee <hoonhee.lee@lge.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>
#include <gst/check/gstcheck.h>
#include <stdlib.h>

GST_START_TEST (test_fakeadec_create)
{
  GstElement *dec;

  dec = gst_element_factory_make ("fakeadec", NULL);
  fail_unless (dec != NULL, "failed to create fakeadec element");

  gst_object_unref (dec);
}

GST_END_TEST;

static Suite *
fakeadec_suite (void)
{
  Suite *s = suite_create ("fakeadec");
  TCase *tc_chain;

  tc_chain = tcase_create ("fakeadec simple");
  tcase_add_test (tc_chain, test_fakeadec_create);
  suite_add_tcase (s, tc_chain);

  return s;
}

GST_CHECK_MAIN (fakeadec);
