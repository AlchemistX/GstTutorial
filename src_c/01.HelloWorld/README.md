# Hello World!
This tutorial is quoted from [HelloWorld](http://docs.gstreamer.com/pages/viewpage.action?pageId=327735)

## Walkthrough

Let's review these lines of code and see what they do:
```c
/* Initialize GStreamer */
gst_init (&argc, &argv);
```

This must always be your first GStreamer command. Among other things, [gst_init()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-Gst.html#gst-init):

  - Initializes all internal structures
  - Checks what plug-ins are available
  - Executes any command-line option intended for GStreamer

If you always pass your command-line parameters argc and argv to gst_init(), your application will automatically benefit from the GStreamer standard command-line options.

```c
  /* Build the pipeline */
  pipeline =
      gst_parse_launch
      ("playbin uri=http://docs.gstreamer.com/media/sintel_trailer-480p.webm",
      NULL);
```

This line is the heart of this tutorial, and exemplifies two key points: [gst_parse_launch()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstParse.html#gst-parse-launch) and [playbin](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/gst-plugins-base-plugins-playbin.html).

### gst_parse_launch
GStreamer is a framework designed to handle multimedia flows. Media travels from the “source” elements (the producers), down to the “sink” elements (the consumers), passing through a series of intermediate elements performing all kinds of tasks. The set of all the interconnected elements is called a “pipeline”.

In GStreamer you usually build the pipeline by manually assembling the individual elements, but, when the pipeline is easy enough, and you do not need any advanced features, you can take the shortcut: [gst_parse_launch()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstParse.html#gst-parse-launch).

This function takes a textual representation of a pipeline and turns it into an actual pipeline, which is very handy. In fact, this function is so handy there is a tool built completely around it which you will get very acquainted with.

### playbin
So, what kind of pipeline are we asking [gst_parse_launch()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstParse.html#gst-parse-launch)to build for us? Here enters the second key point: We are building a pipeline composed of a single element called [playbin](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/gst-plugins-base-plugins-playbin.html).

playbin is a special element which acts as a source and as a sink, and is capable of implementing a whole pipeline. Internally, it creates and connects all the necessary elements to play your media, so you do not have to worry about it.

It does not allow the control granularity that a manual pipeline does, but, still, it permits enough customization to suffice for a wide range of applications. Including this tutorial.

In this example, we are only passing one parameter to playbin, which is the URI of the media we want to play. Try changing it to something else! Whether it is an http:// or file:// URI, playbin will instantiate the appropriate GStreamer source transparently!

If you mistype the URI, or the file does not exist, or you are missing a plug-in, GStreamer provides several notification mechanisms, but the only thing we are doing in this example is exiting on error, so do not expect much feedback.

```c
/* Start playing */
gst_element_set_state (pipeline, GST_STATE_PLAYING);
```

This line highlights another interesting concept: the state.
Every GStreamer element has an associated state, which you can more or less think of as the Play/Pause button in your regular DVD player. For now, suffice to say that playback will not start unless you set the pipeline to the PLAYING state.

In this line, [gst_element_set_state()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElement.html#gst-element-set-state) is setting pipeline (our only element, remember) to the PLAYING state, thus initiating playback.

```c
/* Wait until error or EOS */
bus = gst_element_get_bus (pipeline);
gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
```

These lines will wait until an error occurs or the end of the stream is found. [gst_element_get_bus()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElement.html#gst-element-get-bus) retrieves the pipeline's bus, and [gst_bus_timed_pop_filtered()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstBus.html#gst-bus-timed-pop-filtered) will block until you receive either an ERROR or an EOS (End-Of-Stream) through that bus. Do not worry much about this line, the GStreamer bus is explained in the other tutorial.

And that's it! From this point onwards, GStreamer takes care of everything. Execution will end when the media reaches its end (EOS) or an error is encountered (try closing the video window, or unplugging the network cable). The application can always be stopped by pressing control-C in the console.

### Cleanup
Before terminating the application, though, there is a couple of things we need to do to tidy up correctly after ourselves.
```c
/* Free resources */
if (msg != NULL)
    gst_message_unref (msg);
gst_object_unref (bus);
gst_element_set_state (pipeline, GST_STATE_NULL);
gst_object_unref (pipeline);
```
Always read the documentation of the functions you use, to know if you should free the objects they return after using them.

In this case, gst_bus_timed_pop_filtered() returned a message which needs to be freed with [gst_message_unref()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstMessage.html#gst-message-unref) (more about messages in the other tutorial).

gst_element_get_bus() added a reference to the bus that must be freed with [gst_object_unref()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstObject.html#gst-object-unref). Setting the pipeline to the NULL state will make sure it frees any resources it has allocated. Finally, unreferencing the pipeline will destroy it, and all its contents.

## Conclusion
And so ends your first tutorial with GStreamer. We hope its brevity serves as an example of how powerful this framework is!

Let's recap a bit. Today we have learned:
 - How to initialize GStreamer using [gst_init()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-Gst.html#gst-init)
 - How to quickly build a pipeline from a textual description using [gst_parse_launch()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gstreamer-GstParse.html#gst-parse-launch).
 - How to create an automatic playback pipeline using [playbin](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-plugins/html/gst-plugins-base-plugins-playbin.html).
 - How to signal GStreamer to start playback using [gst_element_set_state()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElement.html#gst-element-set-state).
 - How to sit back and relax, while GStreamer takes care of everything, using [gst_element_get_bus()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstElement.html#gst-element-get-bus) and [gst_bus_timed_pop_filtered()](https://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/GstBus.html#gst-bus-timed-pop-filtered).
The next tutorial will keep introducing more basic GStreamer elements, and show you how to build a pipeline manually.

It has been a pleasure having you here, and see you soon!
