#!/usr/bin/env python3
import sys
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst

def main(argv):
    Gst.init(argv)
    pipeline = Gst.parse_launch ("playbin uri=http://docs.gstreamer.com/media/sintel_trailer-480p.webm")
    pipeline.set_state(Gst.State.PLAYING)
    bus = pipeline.get_bus()
    msg = bus.timed_pop_filtered (Gst.CLOCK_TIME_NONE, 
                                  Gst.MessageType.ERROR or 
                                  Gst.MessageType.EOS)
    if msg :
        print(msg)

    pipeline.set_state(Gst.State.NULL)

if __name__ == "__main__":
    main(sys.argv)
