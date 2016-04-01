# GStreamer Concepts

## Goal

The previous tutorial showed how to build a pipeline automatically. Now we are going to build a pipeline manually by instantiating each element and linking them all together. In the process, we will learn:
 - What is a GStreamer element and how to create one.
 - How to connect elements to each other.
 - How to customize an element's behavior.
 - How to watch the bus for error conditions and extract information from GStreamer messages.

## Walkthrough

The basic construction block of GStreamer are the elements, which process the data as it flows downstream from the source elements (the producers of data) to the sink elements (the consumers of data), passing through filter elements.

<div align="center">
<img src="http://docs.gstreamer.com/download/attachments/327782/figure-1.png?version=1&modificationDate=1333098293703" alt="Example Pipeline"><br>
Figure 1. Example Pipeline
</div>

### Element creation

We will skip GStreamer initialization, since it is the same as the previous tutorial:

```c
/* Create the elements */
source = gst_element_factory_make ("videotestsrc", "source");
sink = gst_element_factory_make ("autovideosink", "sink");
```

As seen in this code, new elements can be created with [gst_element_factory_make()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElementFactory.html#gst-element-factory-make). The first parameter is the type of element to create. The second parameter is the name we want to give to this particular instance. Naming your elements is useful to retrieve them later if you didn't keep a pointer (and for more meaningful debug output). If you pass NULL for the name, however, GStreamer will provide a unique name for you.

For this tutorial we create two elements: a [videotestsrc](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/gst-plugins-base-plugins-videotestsrc.html) and an [autovideosink](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/gst-plugins-good-plugins-autovideosink.html).

[videotestsrc](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/gst-plugins-base-plugins-videotestsrc.html) is a source element (it produces data), which creates a test video pattern. This element is useful for debugging purposes (and tutorials) and is not usually found in real applications.

[autovideosink](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/gst-plugins-good-plugins-autovideosink.html) is a sink element (it consumes data), which displays on a window the images it receives. There exist several video sinks, depending on the operating system, with a varying range of capabilities. autovideosink automatically selects and instantiates the best one, so you do not have to worry with the details, and your code is more platform-independent.

### Pipeline creation

```c
/* Create the empty pipeline */
pipeline = gst_pipeline_new ("test-pipeline");
```

All elements in GStreamer must typically be contained inside a pipeline before they can be used, because it takes care of some clocking and messaging functions. We create the pipeline with [gst_pipeline_new()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstPipeline.html#gst-pipeline-new).

```c
/* Build the pipeline */
gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);
if (gst_element_link (source, sink) != TRUE) {
  g_printerr ("Elements could not be linked.\n");
  gst_object_unref (pipeline);
  return -1;
}
```

A pipeline is a particular type of [bin](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstBin.html), which is the element used to contain other elements. Therefore all methods which apply to bins also apply to pipelines. In our case, we call [gst_bin_add_many()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstBin.html#gst-bin-add-many) to add the elements to the pipeline (mind the cast). This function accepts a list of elements to be added, ending with NULL. Individual elements can be added with [gst_bin_add()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstBin.html#gst-bin-add).

These elements, however, are not linked with each other yet. For this, we need to use [gst_element_link()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElement.html#gst-element-link). Its first parameter is the source, and the second one the destination. The order counts, because links must be established following the data flow (this is, from source elements to sink elements). Keep in mind that only elements residing in the same bin can be linked together, so remember to add them to the pipeline before trying to link them!

### Properties

```c
/* Modify the source's properties */
g_object_set (source, "pattern", 0, NULL);
```

Most GStreamer elements have customizable properties: named attributes that can be modified to change the element's behavior (writable properties) or inquired to find out about the element's internal state (readable properties).

Properties are read from with [g_object_get()](https://developer.gnome.org/gobject/unstable/gobject-The-Base-Object-Type.html#g-object-get) and written to with [g_object_set()](https://developer.gnome.org/gobject/unstable/gobject-The-Base-Object-Type.html#g-object-set).

g_object_set() accepts a NULL-terminated list of property-name, property-value pairs, so multiple properties can be changed in one go (GStreamer elements are all a particular kind of GObject, which is the entity offering property facilities: This is why the property handling methods have the g_ prefix).

The line of code above changes the “pattern” property of videotestsrc, which controls the type of test video the element outputs. Try different values!

The names and possible values of all the properties an element exposes can be found using the gst-inspect tool described in the other tutorial.

### Error checking

At this point, we have the whole pipeline built and setup, and the rest of the tutorial is very similar to the previous one, but we are going to add more error checking:

```c
/* Start playing */
ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
if (ret == GST_STATE_CHANGE_FAILURE) {
  g_printerr ("Unable to set the pipeline to the playing state.\n");
  gst_object_unref (pipeline);
  return -1;
}
```
We call [gst_element_set_state()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElement.html#gst-element-set-state), but this time we check its return value for errors. Changing states is a delicate process and a few more details are given in the other tutorial.

```c
/* Wait until error or EOS */
bus = gst_element_get_bus (pipeline);
msg =
    gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
    GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

/* Parse message */
if (msg != NULL) {
  GError *err;
  gchar *debug_info;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error (msg, &err, &debug_info);
      g_printerr ("Error received from element %s: %s\n",
          GST_OBJECT_NAME (msg->src), err->message);
      g_printerr ("Debugging information: %s\n",
          debug_info ? debug_info : "none");
      g_clear_error (&err);
      g_free (debug_info);
      break;
    case GST_MESSAGE_EOS:
      g_print ("End-Of-Stream reached.\n");
      break;
    default:
      /* We should not reach here because we only asked for ERRORs and EOS */
      g_printerr ("Unexpected message received.\n");
      break;
  }
  gst_message_unref (msg);
}
```

[gst_bus_timed_pop_filtered()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstBus.html#gst-bus-timed-pop-filtered) waits for execution to end and returns with a GstMessage which we previously ignored. We asked gst_bus_timed_pop_filtered() to return when GStreamer encountered either an error condition or an EOS, so we need to check which one happened, and print a message on screen (Your application will probably want to undertake more complex actions).

[GstMessage](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstMessage.html) is a very versatile structure which can deliver virtually any kind of information. Fortunately, GStreamer provides a series of parsing functions for each kind of message.

In this case, once we know the message contains an error (by using the [GST_MESSAGE_TYPE()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstMessage.html#GST-MESSAGE-TYPE:CAPS) macro), we can use [gst_message_parse_error()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstMessage.html#gst-message-parse-error) which returns a GLib [GError](https://developer.gnome.org/glib/stable/glib-Error-Reporting.html) error structure and a string useful for debugging. Examine the code to see how these are used and freed afterward. 

### The GStreamer bus

At this point it is worth introducing the GStreamer bus a bit more formally. It is the object responsible for delivering to the application the GstMessages generated by the elements, in order and to the application thread. This last point is important, because the actual streaming of media is done in another thread than the application.

Messages can be extracted from the bus synchronously with gst_bus_timed_pop_filtered() and its siblings, or asynchronously, using signals (shown in the next tutorial). Your application should always keep an eye on the bus to be notified of errors and other playback-related issues.

The rest of the code is the cleanup sequence, which is the same as in Basic tutorial 01: Hello world!.

## Exercise

If you feel like practicing, try this exercise: Add a video filter element in between the source and the sink of this pipeline. Use [vertigotv](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/gst-plugins-good-plugins-vertigotv.html) for a nice effect. You will need to create it, add it to the pipeline, and link it with the other elements.

Depending on your platform and available plugins, you might get a “negotiation” error, because the sink does not understand what the filter is producing. In this case, try to add an element called [videoconvert](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/gst-plugins-base-plugins-videoconvert.html) after the filter.

## Conclusion

This tutorial showed:

 - How to create elements with [gst_element_factory_make()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElementFactory.html#gst-element-factory-make)
 - How to create an empty pipeline with [gst_pipeline_new()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstPipeline.html#gst-pipeline-new)
 - How to add elements to the pipeline with [gst_bin_add_many()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstBin.html#gst-bin-add-many)
 - How to link the elements with each other with [gst_element_link()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElement.html#gst-element-link)

This concludes the first of the two tutorials devoted to basic GStreamer concepts. The second one comes next.

Remember that attached to this page you should find the complete source code of the tutorial and any accessory files needed to build it.

It has been a pleasure having you here, and see you soon!
