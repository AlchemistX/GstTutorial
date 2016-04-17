#!/usr/bin/env python3
import sys
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst

def main(argv):
    Gst.init(argv)

    #Create the elements
    source = Gst.ElementFactory.make ("videotestsrc", "source");
    sink = Gst.ElementFactory.make ("autovideosink", "sink");

    # Create the empty pipeline
    pipeline = Gst.Pipeline.new ("test-pipeline");

    if pipeline == None or source == None or sink == None :
        print("Not all elements could be created.\n")
        sys.exit(-1)

    # Build the pipeline
    pipeline.add(source)
    pipeline.add(sink)
    if source.link(sink) != True :
        print("Elements could not be linked.\n")
        sys.exit(-1)

    # Modify the source's properties
    source.set_property("pattern", 0)

    # Start playing
    if pipeline.set_state(Gst.State.PLAYING) == Gst.StateChangeReturn.FAILURE :
        print("Unable to set the pipeline to the playing state.\n")
        sys.exit(-1)

    # Wait until error or EOS
    bus = pipeline.get_bus()
    msg = bus.timed_pop_filtered (Gst.CLOCK_TIME_NONE,
                                  Gst.MessageType.ERROR |
                                  Gst.MessageType.EOS)
    # Parse message
    if msg :
        if msg.type == Gst.MessageType.ERROR:
            err, debug = msg.parse_error()
            print(("Error received from element %s: %s\n")%(msg.src, err.message))
            print(("Debugging information %s\n")%(debug))
        elif msg.type == Gst.MessageType.EOS:
            print("End-Of-Stream reached.\n")
        else:
            print("Unexpected message received.\n")

    # Free resources
    pipeline.set_state(Gst.State.NULL)

if __name__ == "__main__":
    main(sys.argv)
