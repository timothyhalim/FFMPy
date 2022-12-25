
import sys
import os

lib = os.path.join(__file__, "..", "..", "build", "Release")
if os.path.exists(lib):
    lib = os.path.normpath(lib)
    if not lib in sys.path:
        sys.path.append(lib)

import FFMPy

from PySide2.QtWidgets import QApplication, QWidget, QLabel, QVBoxLayout, QSlider
from PySide2.QtGui import QPixmap, QImage
from PySide2.QtCore import Qt, QTimer

from time import time

class Widget(QWidget):
    def __init__(self, parent=None):
        super(Widget, self).__init__(parent)
        
    def show(self):
        super().show()

        self.decoder = FFMPy.Decoder(os.path.join(__file__, "..", "video.mp4"))
        self.info = self.decoder.get_input_info()
        print(self.info)
        
        layout = QVBoxLayout(self)
        self.label = QLabel(self)
        layout.addWidget(self.label)
        self.slider = QSlider(self)
        layout.addWidget(self.slider)
        self.slider.setMaximum(self.info['video_frame_count']-1)
        self.slider.setOrientation(Qt.Horizontal)
        self.slider.valueChanged.connect(self.change_picture)
        self.change_picture(0)

        self.timer = QTimer(self)
        self.timer.setInterval(1000/(self.info['video_frame_rate'] or 1) )
        self.timer.timeout.connect(self.next_frame)

        self.decoder.open_stream()
        self.start_time = time()
        self.timer.start()

    def next_frame(self):
        current_time = int((time()-self.start_time)*self.info['video_frame_rate'])
        if current_time > self.info['video_frame_count']:
            self.start_time = time()
        self.slider.setValue(current_time)

    def change_picture(self, frame):
        frame = self.decoder.extract_frame(frame)
        pix = QImage(frame, self.info['video_width'], self.info['video_height'], QImage.Format_RGBA8888)
        self.label.setPixmap(QPixmap.fromImage(pix).scaledToWidth(800, Qt.SmoothTransformation))
        
if __name__ == "__main__":
    app = QApplication(sys.argv)
    w = Widget()
    w.show()
    app.exec_()

