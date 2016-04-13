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

GST_START_TEST (test_fakeadec_input_output_caps)
{
  GstElement *dec;
  GstPad *sinkpad, *srcpad;
  GstCaps *caps1;
  GstCaps *tmp_caps;

  dec = gst_element_factory_make ("fakeadec", NULL);
  fail_unless (dec != NULL, "failed to create fakeadec element");

  sinkpad = gst_element_get_static_pad (dec, "sink");
  fail_unless (dec != NULL, "failed to get sinkpad");

  tmp_caps = gst_pad_get_pad_template_caps (sinkpad);
  fail_unless (!gst_caps_is_any (tmp_caps), "sinkpad caps is any");
  caps1 = gst_caps_from_string ("audio/mpeg");
  fail_unless (gst_caps_can_intersect (tmp_caps, caps1),
      "sinkpad can not accept 'audio/mpeg'");
  gst_caps_unref (caps1);
  gst_caps_unref (tmp_caps);
  gst_object_unref (sinkpad);

  srcpad = gst_element_get_static_pad (dec, "src");
  fail_unless (dec != NULL, "failed to get srcpad");

  tmp_caps = gst_pad_get_pad_template_caps (srcpad);
  caps1 = gst_caps_from_string ("audio/x-raw");
  fail_unless (gst_caps_is_equal (tmp_caps, caps1),
      "sinkpad can not accept 'audio/mpeg'");
  gst_caps_unref (caps1);
  gst_caps_unref (tmp_caps);
  gst_object_unref (srcpad);

  gst_object_unref (dec);
}

GST_END_TEST;

static GstFlowReturn
chain_probe (GstPad * pad, GstObject * parent, GstBuffer * buffer)
{
  fail_unless (GST_BUFFER_FLAG_IS_SET (buffer, GST_BUFFER_FLAG_CORRUPTED),
      "Invalid buffer");
  gst_buffer_unref (buffer);

  return GST_FLOW_OK;
}

GST_START_TEST (test_fakeadec_buffer)
{
  GstElement *dec;
  GstPad *pad;
  GstPad *mysrc, *mysink;
  GstCaps *mycaps;

  dec = gst_element_factory_make ("fakeadec", NULL);
  fail_unless (dec != NULL, "failed to create fakeadec element");
  fail_unless (gst_element_set_state (dec, GST_STATE_PLAYING) ==
      GST_STATE_CHANGE_SUCCESS);

  GST_DEBUG ("Creating mysink");
  mysink = gst_pad_new ("mysink", GST_PAD_SINK);
  gst_pad_set_chain_function (mysink, chain_probe);
  gst_pad_set_active (mysink, TRUE);
  pad = gst_element_get_static_pad (dec, "src");
  fail_unless (GST_PAD_LINK_SUCCESSFUL (gst_pad_link (pad, mysink)));
  gst_object_unref (pad);

  GST_DEBUG ("Creating mysrc");
  mysrc = gst_pad_new ("mysrc", GST_PAD_SRC);
  gst_pad_set_active (mysrc, TRUE);
  pad = gst_element_get_static_pad (dec, "sink");
  fail_unless (GST_PAD_LINK_SUCCESSFUL (gst_pad_link (mysrc, pad)));
  gst_object_unref (pad);

  GST_DEBUG ("Pushing stream-start, caps and segment event");
  mycaps = gst_caps_new_empty_simple ("audio/mpeg");
  gst_check_setup_events_with_stream_id (mysrc, dec, mycaps,
      GST_FORMAT_BYTES, "test0");
  fail_unless (gst_pad_push (mysrc, gst_buffer_new ()) == GST_FLOW_OK);

  GST_DEBUG ("Releasing");
  fail_unless (gst_element_set_state (dec, GST_STATE_NULL) ==
      GST_STATE_CHANGE_SUCCESS);

  gst_pad_set_active (mysink, FALSE);
  gst_pad_set_active (mysrc, FALSE);

  gst_object_unref (mysink);
  gst_object_unref (mysrc);

  gst_caps_unref (mycaps);
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
  tcase_add_test (tc_chain, test_fakeadec_input_output_caps);
  tcase_add_test (tc_chain, test_fakeadec_buffer);
  suite_add_tcase (s, tc_chain);

  return s;
}

GST_CHECK_MAIN (fakeadec);
