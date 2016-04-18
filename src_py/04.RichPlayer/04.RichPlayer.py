#!/usr/bin/env python3
import sys
import gi
gi.require_version('Gst', '1.0')
gi.require_version('Gtk', '3.0')
gi.require_version('GLib', '2.0')
gi.require_version('GstVideo', '1.0')
from gi.repository import Gst, Gtk, GLib, GObject
from gi.repository import GdkX11, GstVideo

class GTK_Main(object):
    def __init__(self):
        ################ GTK #######################
        main_window = Gtk.Window(type=Gtk.WindowType.TOPLEVEL)
        main_window.connect("destroy", Gtk.main_quit, "WM destroy")
        self.video_window = Gtk.DrawingArea()

        play_button = Gtk.Button.new_from_stock(Gtk.STOCK_MEDIA_PLAY)
        play_button.connect("clicked", self.on_play, None)

        pause_button = Gtk.Button.new_from_stock(Gtk.STOCK_MEDIA_PAUSE)
        pause_button.connect("clicked", self.on_pause, None)

        stop_button = Gtk.Button.new_from_stock(Gtk.STOCK_MEDIA_STOP)
        stop_button.connect("clicked", self.on_stop, None)

        self.slider = Gtk.HScale.new_with_range(0, 100, 1)
        self.slider.set_draw_value(0);
        self.slider_update_sig_id = self.slider.connect("value-changed",
                                                        self.on_slider, None)

        self.streams_list = Gtk.TextView()
        self.streams_list.set_editable(False);

        controls = Gtk.HBox(False, 0)
        controls.pack_start(play_button, False, False, 2)
        controls.pack_start(pause_button, False, False, 2)
        controls.pack_start(stop_button, False, False, 2)
        controls.pack_start(self.slider, True, True, 2)

        main_hbox = Gtk.HBox(False, 0)
        main_hbox.pack_start(self.video_window, True, True, 0)
        main_hbox.pack_start(self.streams_list, False, False, 2)

        main_box = Gtk.VBox(False, 0)
        main_box.pack_start(main_hbox, True, True, 0)
        main_box.pack_start(controls, False, False, 0)

        main_window.add(main_box)
        main_window.set_default_size(640, 480)

        main_window.show_all()

        ################ GST #######################
        self.state = Gst.State.NULL
        self.duration = Gst.CLOCK_TIME_NONE
        self.tag_str = ""
        self.uri = ""
        self.player = Gst.ElementFactory.make("playbin", "player")
        #self.player.set_property("uri",
        #            "http://docs.gstreamer.com/media/sintel_trailer-480p.webm")
        self.player.connect("video-tags-changed", self.on_tags)
        self.player.connect("audio-tags-changed", self.on_tags)
        self.player.connect("text-tags-changed", self.on_tags)

        # Add a bin such as | [ghost sink pad]->time overlay->autovideosink |
        bin = Gst.Bin.new("my-bin")
        timeoverlay = Gst.ElementFactory.make("timeoverlay")
        bin.add(timeoverlay)
        pad = timeoverlay.get_static_pad("video_sink")
        ghostpad = Gst.GhostPad.new("sink", pad)
        bin.add_pad(ghostpad)
        videosink = Gst.ElementFactory.make("autovideosink")
        bin.add(videosink)
        timeoverlay.link(videosink)
        self.player.set_property("video-sink", bin)

        bus = self.player.get_bus()
        bus.add_signal_watch()
        bus.enable_sync_message_emission()
        bus.connect("sync-message::element", self.on_sync_message)
        bus.connect("message::error", self.on_error)
        bus.connect("message::eos", self.on_eos)
        bus.connect("message::state-changed", self.on_state_changed)
        bus.connect("message::application", self.on_application)

        GLib.timeout_add_seconds(1, self.on_refresh_ui, None)

    def on_tags(self, player, data):
        tag_str = ""

        n_video = player.get_property("n-video")
        n_audio = player.get_property("n-audio")
        n_text = player.get_property("n-text")

        for i in range(n_video):
            tags = player.emit("get-video-tags", i)
            if tags:
                tag_str += ("video stream : %d\n")%(i)
                ret, codec = tags.get_string(Gst.TAG_VIDEO_CODEC)
                if ret:
                    tag_str += ("    codec: %s\n")%(codec)
                else:
                    tag_str += ("    codec: unknown\n")

        for i in range(n_audio):
            tags = player.emit("get-audio-tags", i)
            if tags:
                tag_str += ("audio stream : %d\n")%(i)
                ret, codec = tags.get_string(Gst.TAG_AUDIO_CODEC)
                if ret:
                    tag_str += ("    codec: %s\n")%(codec)
                ret, code  = tags.get_string(Gst.TAG_LANGUAGE_CODE)
                if ret:
                    tag_str += (" language: %s\n")%(code)
                ret, rate  = tags.get_uint(Gst.TAG_BITRATE)
                if ret:
                    tag_str += ("  bitrate: %d\n")%(rate)

        for i in range(n_text):
            tags = player.emit("get-text-tags", i)
            if tags:
                tag_str += ("subtitle stream : %d\n")%(i)
                ret, code  = tags.get_string(Gst.TAG_LANGUAGE_CODE)
                if ret:
                    tag_str += (" language: %s\n")%(code)

        self.tag_str = tag_str


    def on_sync_message(self, bus, message):
        if message.get_structure().get_name() == 'prepare-window-handle':
            imagesink = message.src
            imagesink.set_property("force-aspect-ratio", True)
            imagesink.set_window_handle(
                            self.video_window.get_property('window').get_xid())

    def on_error(self, bus, message):
        err, debug = message.parse_error()
        print(("Error received from element %s: %s")%(message.src, err.message))
        print(("Debugging information %s")%(debug))

    def on_eos(self, bus, message):
        print("End-Of-Stream reached.")
        self.player.set_state(Gst.State.READY)

    def on_state_changed(self, bus, message):
        old_state, new_state, pending_state = message.parse_state_changed()
        if message.src == self.player:
            self.state = new_state
            print(("State set to %s")%(new_state))
            if old_state == Gst.State.READY and new_state == Gst.State.PAUSED:
                self.on_refresh_ui(None)
            elif new_state == Gst.State.PLAYING:
                if self.tag_str != "":
                    print(self.tag_str)
                    txt = self.streams_list.get_buffer()
                    txt.set_text(self.tag_str)
                    self.tag_str = ""

    def on_application(self, bus, message):
        if message.get_structure().get_name() == 'tags-changed':
            print("Updating tag!!")

    def on_play(self, obj, data):
        if self.uri == "":
            dlg = Gtk.FileChooserDialog(action=Gtk.FileChooserAction.OPEN)
            dlg.add_buttons(Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL)
            dlg.add_buttons(Gtk.STOCK_OPEN, Gtk.ResponseType.OK)
            res = dlg.run()
            if res == Gtk.ResponseType.OK:
                self.uri = "file://" + dlg.get_filename()
                dlg.destroy()
                self.player.set_property("uri", self.uri);
                self.player.set_state(Gst.State.PLAYING)
        else:
            self.player.set_state(Gst.State.PLAYING)

    def on_pause(self, obj, data):
        self.player.set_state(Gst.State.PAUSED)

    def on_stop(self, obj, data):
        self.player.set_state(Gst.State.READY)
        self.uri = ""

    def on_slider(self, obj, data):
        val = self.slider.get_value()
        self.player.seek_simple(Gst.Format.TIME,
                                Gst.SeekFlags.FLUSH | Gst.SeekFlags.KEY_UNIT,
                                val*Gst.SECOND)

    def on_refresh_ui(self, data):
        if self.state < Gst.State.PAUSED :
            return True

        if self.duration == Gst.CLOCK_TIME_NONE :
            ret, self.duration = self.player.query_duration(Gst.Format.TIME)
            if ret:
                self.slider.set_range(0, self.duration/Gst.SECOND)
            else:
                print("Could not query current duration.")

        ret, pos = self.player.query_position(Gst.Format.TIME)
        if ret:
            GObject.signal_handler_block(self.slider,
                                         self.slider_update_sig_id)
            self.slider.set_value(pos/Gst.SECOND)
            GObject.signal_handler_unblock(self.slider,
                                           self.slider_update_sig_id)

        return True

if __name__ == "__main__":
    Gst.init(sys.argv)
    GTK_Main()
    Gtk.main()
