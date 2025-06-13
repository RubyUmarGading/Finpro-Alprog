import tkinter as tk
from tkinter import ttk
import time
from datetime import datetime
import threading
import subprocess

# Constants
TANK_HEIGHT_CM = 200
CANVAS_HEIGHT = 400
CANVAS_WIDTH = 300
WATER_COLOR = "#00BFFF"
TANK_COLOR = "#A9A9A9"

# Scaling factor
SCALE = CANVAS_HEIGHT / TANK_HEIGHT_CM

class WaterTankSimulator:
    def __init__(self, root):
        self.root = root
        self.root.title("Simulasi Tangki Air")

        self.canvas = tk.Canvas(root, width=CANVAS_WIDTH, height=CANVAS_HEIGHT, bg="white")
        self.canvas.pack(pady=10)

        # Create the slider without binding the command yet
        self.slider = ttk.Scale(root, from_=TANK_HEIGHT_CM, to=0, orient='vertical', length=CANVAS_HEIGHT)
        self.slider.set(100)
        self.slider.pack(side='right', padx=20)

        self.draw_tank()
        self.draw_ruler()

        # Create initial water level rectangle
        self.water_rect = self.canvas.create_rectangle(100, CANVAS_HEIGHT, 200, CANVAS_HEIGHT, fill=WATER_COLOR)

        # Now bind the update function after the rectangle exists
        self.slider.configure(command=self.update_water_level)

        # Start background thread for sending data
        self.running = True
        threading.Thread(target=self.send_water_level_periodically, daemon=True).start()

        # Initial draw
        self.update_water_level(self.slider.get())

    def draw_tank(self):
        self.canvas.create_rectangle(100, 0, 200, CANVAS_HEIGHT, fill=TANK_COLOR)

    def draw_ruler(self):
        for i in range(0, TANK_HEIGHT_CM + 1, 20):
            y = CANVAS_HEIGHT - i * SCALE
            self.canvas.create_line(90, y, 100, y)
            self.canvas.create_text(80, y, text=str(i), anchor='e', font=("Arial", 8))

    def update_water_level(self, value):
        try:
            value = float(value)
            height = value * SCALE
            y1 = CANVAS_HEIGHT - height
            if self.water_rect:  # ensure rectangle exists
                self.canvas.coords(self.water_rect, 100, y1, 200, CANVAS_HEIGHT)
        except Exception as e:
            print("Update error:", e)

    def send_water_level_periodically(self):
        while self.running:
            level = int(float(self.slider.get()))
            try:
                import os
                exe_path = os.path.join(os.path.dirname(__file__), "clientNew.exe")
                subprocess.run([exe_path, str(level)], check=True)
                print(f"Sent level {level} to server.")
            except Exception as e:
                print("Failed to send data:", e)
            time.sleep(1)

    def stop(self):
        self.running = False

root = tk.Tk()
simulator = WaterTankSimulator(root)

def on_closing():
    simulator.stop()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)
root.mainloop()
