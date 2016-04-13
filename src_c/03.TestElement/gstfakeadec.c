/* GStreamer FakeADEC element
 * Copyright (C) 2016 LG Electronics, Inc.
 *    Author : HoonHee Lee <hoonhee.lee@lge.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstfakeadec.h"

static GstStaticPadTemplate gst_fakeadec_sink_pad_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate gst_fakeadec_src_pad_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC (fakeadec_debug);
#define GST_CAT_DEFAULT fakeadec_debug

#define gst_fakeadec_parent_class parent_class
G_DEFINE_TYPE (GstFakeAdec, gst_fakeadec, GST_TYPE_ELEMENT);

static void
gst_fakeadec_class_init (GstFakeAdecClass * klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_fakeadec_src_pad_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_fakeadec_sink_pad_template));

  gst_element_class_set_static_metadata (element_class, "Fake Audio decoder",
      "Codec/Decoder/Audio",
      "Pass data to backend decoder", "HoonHee Lee <hoonhee.lee@lge.com>");

  GST_DEBUG_CATEGORY_INIT (fakeadec_debug, "fakeadec", 0, "Fake audio decoder");
}

static void
gst_fakeadec_init (GstFakeAdec * fakeadec)
{
  fakeadec->sinkpad =
      gst_pad_new_from_static_template (&gst_fakeadec_sink_pad_template,
      "sink");
  gst_element_add_pad (GST_ELEMENT (fakeadec), fakeadec->sinkpad);

  fakeadec->srcpad =
      gst_pad_new_from_static_template (&gst_fakeadec_src_pad_template, "src");
  gst_element_add_pad (GST_ELEMENT (fakeadec), fakeadec->srcpad);
}
