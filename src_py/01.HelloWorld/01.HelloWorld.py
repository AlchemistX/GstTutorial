#!/usr/bin/env python3
import sys
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst

import os
os.environ['GST_DEBUG_DUMP_DOT_DIR'] = '.'


def main(argv):
    Gst.init(argv)
    pipeline = Gst.parse_launch("playbin videosink=glimagesink uri=http://docs.gstreamer.com/media/sintel_trailer-480p.webm")
    pipeline.set_state(Gst.State.PLAYING)
    bus = pipeline.get_bus()
    while 1:
        msg = bus.timed_pop_filtered(Gst.CLOCK_TIME_NONE,
                                     Gst.MessageType.NEW_CLOCK |
                                     Gst.MessageType.CLOCK_LOST |
                                     Gst.MessageType.EOS |
                                     Gst.MessageType.TAG |
                                     Gst.MessageType.TOC |
                                     Gst.MessageType.INFO |
                                     Gst.MessageType.WARNING |
                                     Gst.MessageType.ERROR |
                                     Gst.MessageType.STATE_CHANGED |
                                     Gst.MessageType.BUFFERING |
                                     Gst.MessageType.LATENCY |
                                     Gst.MessageType.APPLICATION |
                                     Gst.MessageType.PROGRESS |
                                     Gst.MessageType.ELEMENT |
                                     Gst.MessageType.HAVE_CONTEXT)
        if msg is not None:
            if msg.get_structure() is not None:
                print(msg.get_structure().to_string())

    pipeline.set_state(Gst.State.NULL)

if __name__ == "__main__":
    main(sys.argv)
