/* sample application for testing fakeadec with playbin
 *
 * Copyright 2016 LGE Corporation
 *  @author: HoonHee Lee <hoonhee.lee@lge.com>
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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>
#include <glib-object.h>
#include <glib/gprintf.h>
#include <gst/gst.h>

/* Global structure */

typedef struct _MyDataStruct
{
  GMainLoop *mainloop;
  GstElement *pipeline;
} MyDataStruct;

static GstBusSyncReply
_on_bus_message (GstBus * bus, GstMessage * message, MyDataStruct * data)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ASYNC_DONE:
      // export GST_DEBUG_DUMP_DOT_DIR=/home/hoonheelee/work/dot_graph
      GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (data->pipeline),
          GST_DEBUG_GRAPH_SHOW_ALL, "async-done");
      break;
    case GST_MESSAGE_ERROR:{
      GError *err = NULL;
      gchar *name = gst_object_get_path_string (GST_MESSAGE_SRC (message));
      gst_message_parse_error (message, &err, NULL);

      g_printerr ("ERROR: from element %s: %s\n", name, err->message);
      g_error_free (err);
      g_free (name);

      g_printf ("Stopping\n");
      g_main_loop_quit (data->mainloop);
      break;
    }
    case GST_MESSAGE_EOS:
      g_printf ("EOS ! Stopping \n");
      g_main_loop_quit (data->mainloop);
      break;
    default:
      break;
  }

  return GST_BUS_PASS;
}

static gchar *
cmdline_to_uri (const gchar * arg)
{
  if (gst_uri_is_valid (arg))
    return g_strdup (arg);

  return gst_filename_to_uri (arg, NULL);
}

int
main (int argc, gchar ** argv)
{
  GstBus *bus;
  MyDataStruct *data;
  gchar *uri;
  GstPluginFeature *feature;

  gst_init (&argc, &argv);

  feature = gst_registry_find_feature (gst_registry_get (), "fakeadec",
      GST_TYPE_ELEMENT_FACTORY);
  gst_plugin_feature_set_rank (feature, GST_RANK_PRIMARY + 100);

  data = g_new0 (MyDataStruct, 1);

  uri = cmdline_to_uri (argv[1]);

  if (argc < 2 || uri == NULL) {
    g_print ("Usage: %s URI\n", argv[0]);
    return 1;
  }

  data->pipeline = gst_element_factory_make ("playbin", NULL);
  if (data->pipeline == NULL) {
    g_printerr ("Failed to create playbin element. Aborting");
    return 1;
  }

  g_object_set (data->pipeline, "uri", uri, NULL);
  g_free (uri);

  {
    GstElement *sink = gst_element_factory_make ("fakesink", NULL);
    g_object_set (sink, "sync", FALSE, NULL);
    g_object_set (data->pipeline, "audio-sink", sink, NULL);
  }

  data->mainloop = g_main_loop_new (NULL, FALSE);

  /* Put a bus handler */
  bus = gst_pipeline_get_bus (GST_PIPELINE (data->pipeline));
  gst_bus_set_sync_handler (bus, (GstBusSyncHandler) _on_bus_message, data,
      NULL);

  /* Start pipeline */
  gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
  g_main_loop_run (data->mainloop);

  gst_element_set_state (data->pipeline, GST_STATE_NULL);

  gst_object_unref (data->pipeline);
  gst_object_unref (bus);

  /* don't want to interfere with any other of the other tests */
  gst_plugin_feature_set_rank (feature, GST_RANK_NONE);
  gst_object_unref (feature);

  return 0;
}
