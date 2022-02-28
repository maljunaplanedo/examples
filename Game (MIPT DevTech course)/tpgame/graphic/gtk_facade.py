import gi
import cairo
import math
from tpgame.graphic.grafics_facade import GraphicsFacade
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Gdk, GObject


class GtkCairoFacade(GraphicsFacade):

    CELL_SIZE = 50
    LINE_HEIGHT = 40
    LINE_WIDTH = 640 / 3
    GOLD_PANEL_HEIGHT = 40
    GOLD_PANEL_WIDTH = 300

    def __init__(self, window) -> None:
        super().__init__(window)
        self.gtk_window = Gtk.Window()
        self.gtk_window.connect("destroy", self.close)
        self.gtk_window.set_default_size(self.window.WIDTH, self.window.HEIGHT)
        self.gtk_window.set_resizable(False)
        self.gtk_window.connect("key-press-event", self.keyboard_event)
        self.gtk_draw_area = Gtk.DrawingArea()
        self.gtk_draw_area.connect("draw", self.draw)
        self.gtk_draw_area.show()
        self.gtk_window.add(self.gtk_draw_area)
        self.gtk_window.present()
        self.cr = None

    def iteration(self) -> None:
        while Gtk.events_pending():
            Gtk.main_iteration()

    def keyboard_event(
        self, widget: gi.overrides.Gtk.Window, event: gi.overrides.Gdk.EventKey
    ) -> None:
        key = Gdk.keyval_name(event.keyval)
        self.window.keyboard_event(key)

    def close(self, event) -> None:
        self.window.close()

    def draw(
        self, widget: gi.overrides.Gtk.Window, event: gi.overrides.Gdk.EventKey
    ) -> None:
        self.cr = self.gtk_draw_area.get_window().cairo_create()
        self.window.draw()

    def redraw(self) -> None:
        rect = self.gtk_draw_area.get_allocation()
        self.gtk_draw_area.get_window().invalidate_rect(rect, True)

    def draw_background(self, black: bool = False) -> None:
        if black:
            self.cr.set_source_rgb(0, 0, 0)
        else:
            self.cr.set_source_rgb(1, 1, 1)
        self.cr.rectangle(0, 0, self.window.width, self.window.height)
        self.cr.fill()

    def draw_ground(self, x1: int, y1: int, x2: int, y2: int) -> None:

        width = (x2 - x1 + 1) * self.CELL_SIZE
        height = (y2 - y1 + 1) * self.CELL_SIZE
        bg_src = "images/background.png"
        image_surface = cairo.ImageSurface.create_from_png(bg_src)
        img_height = image_surface.get_height()
        img_width = image_surface.get_width()

        width_ratio = float(width) / float(img_width)
        height_ratio = float(height) / float(img_height)

        self.cr.save()
        self.cr.translate(x1 * self.CELL_SIZE, y1 * self.CELL_SIZE)
        self.cr.scale(width_ratio, height_ratio)
        self.cr.set_source_surface(image_surface)
        self.cr.paint()
        self.cr.restore()

    def draw_squad(self, owner: int, x: int, y: int) -> None:
        if owner == 1:
            avatar = "images/1.png"
        else:
            avatar = "images/2.png"
        self.cr.save()
        self.cr.translate(x * self.CELL_SIZE, y * self.CELL_SIZE)
        image_surface = cairo.ImageSurface.create_from_png(avatar)
        img_height = image_surface.get_height()
        img_width = image_surface.get_width()
        width_ratio = float(self.CELL_SIZE) / float(img_width)
        height_ratio = float(self.CELL_SIZE) / float(img_height)
        self.cr.scale(width_ratio, height_ratio)
        self.cr.set_source_surface(image_surface)
        self.cr.paint()
        self.cr.restore()

    def draw_fortress(self, owner: int, x: int, y: int) -> None:
        if owner == 1:
            castle = "images/castle1.png"
        elif owner == 0:
            castle = "images/castle2.png"
        else:
            castle = "images/castle0.png"

        self.cr.save()
        self.cr.translate(x * self.CELL_SIZE, y * self.CELL_SIZE)
        image_surface = cairo.ImageSurface.create_from_png(castle)
        img_height = image_surface.get_height()
        img_width = image_surface.get_width()
        width_ratio = float(self.CELL_SIZE) / float(img_width)
        height_ratio = float(self.CELL_SIZE) / float(img_height)
        self.cr.scale(width_ratio, height_ratio)
        self.cr.set_source_surface(image_surface)
        self.cr.paint()
        self.cr.restore()

    def draw_line_background(self, col: int,
                             row: int, is_selected: int) -> None:
        if is_selected:
            self.cr.set_source_rgb(1, 1, 1)
        else:
            self.cr.set_source_rgb(0, 0, 0)
        self.cr.rectangle(
            col * self.LINE_WIDTH,
            row * self.LINE_HEIGHT,
            self.LINE_WIDTH,
            self.LINE_HEIGHT,
        )
        self.cr.fill()

    def draw_line_text(self, col: int, row: int, text: str, pos: int) -> None:
        if pos == 0:
            self.cr.set_source_rgb(0, 1, 0)
        elif pos == 1:
            self.cr.set_source_rgb(1, 0, 0)
        elif pos == 2:
            self.cr.set_source_rgb(0, 0, 1)
        else:
            self.cr.set_source_rgb(1, 1, 0)

        self.cr.move_to(
            col * self.LINE_WIDTH + pos * self.LINE_WIDTH / 4,
            row * self.LINE_HEIGHT + self.LINE_HEIGHT / 2,
        )

        self.cr.show_text(text)

    def draw_end_text(self, text: str) -> None:
        self.cr.set_font_size(30)
        self.cr.move_to(self.window.width / 2, self.window.height / 2)
        self.cr.show_text(text)

    def draw_panel_background(self) -> None:
        self.cr.set_source_rgb(0, 0, 0)
        self.cr.rectangle(
            self.window.width - self.GOLD_PANEL_WIDTH,
            self.window.height - self.GOLD_PANEL_HEIGHT,
            self.GOLD_PANEL_WIDTH,
            self.GOLD_PANEL_HEIGHT,
        )
        self.cr.fill()

    def draw_gold(self, gold: int) -> None:
        self.cr.move_to(
            self.window.width - self.GOLD_PANEL_WIDTH + 3,
            self.window.height - self.GOLD_PANEL_HEIGHT / 2,
        )
        self.cr.set_source_rgb(1, 1, 0)
        self.cr.set_font_size(24)
        self.cr.show_text(str(gold))

    def draw_moves_left(self, moves: int) -> None:
        self.cr.move_to(
            self.window.width - self.GOLD_PANEL_WIDTH + 200,
            self.window.height - self.GOLD_PANEL_HEIGHT / 2,
        )
        self.cr.set_source_rgb(0, 0, 1)
        self.cr.set_font_size(24)
        self.cr.show_text(str(moves))

    def draw_target(self, row: int, col: int) -> None:
        self.cr.set_source_rgb(0, 1, 0)
        self.cr.arc(
            row * self.CELL_SIZE + self.CELL_SIZE // 2,
            col * self.CELL_SIZE + self.CELL_SIZE // 2,
            5,
            0,
            2 * math.pi,
        )
        self.cr.fill()

