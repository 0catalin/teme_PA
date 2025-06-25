import tkinter as tk

CELL_SIZE = 50  # bigger cells for better visibility
GRID_LINE_WIDTH = 1  # thinner lines

def move_point(x, y, direction):
    if direction == 'N':
        return (x, y + 1)
    elif direction == 'S':
        return (x, y - 1)
    elif direction == 'E':
        return (x + 1, y)
    elif direction == 'V' or direction == 'W':
        return (x - 1, y)
    else:
        return (x, y)

def parse_input(input_text):
    lines = input_text.strip().split('\n')
    T, N = map(int, lines[0].split())
    mx, my = map(int, lines[1].split())
    logs = []
    for i in range(N):
        x1, y1, x2, y2 = map(int, lines[2 + i].split())
        logs.append(((x1, y1), (x2, y2)))
    # Each log has its own movement string
    directions = lines[2 + N:2 + N + N]
    return T, N, (mx, my), logs, directions

def get_log_points(log):
    (x1, y1), (x2, y2) = log
    points = []
    if x1 == x2:
        ys = range(min(y1, y2), max(y1, y2) + 1)
        points = [(x1, y) for y in ys]
    elif y1 == y2:
        xs = range(min(x1, x2), max(x1, x2) + 1)
        points = [(x, y1) for x in xs]
    else:
        raise ValueError("Logs must be horizontal or vertical")
    return points

def move_log_points(points, direction):
    return [move_point(x, y, direction) for x, y in points]

def compute_bounds(all_points, marian):
    xs = [marian[0]] + [p[0] for points in all_points for p in points]
    ys = [marian[1]] + [p[1] for points in all_points for p in points]
    return min(xs), max(xs), min(ys), max(ys)

class LogSimulatorGUI:
    def __init__(self, root, input_text):
        self.root = root
        self.T, self.N, self.marian_pos, self.logs, self.directions = parse_input(input_text)
        self.logs_points = [get_log_points(log) for log in self.logs]

        self.current_time = 0

        # Precompute all log positions for all timestamps
        self.all_log_positions = self.precompute_log_positions()

        # Calculate grid bounds to fit everything including Marian, add some padding
        min_x, max_x, min_y, max_y = compute_bounds([pts[0] for pts in self.all_log_positions], self.marian_pos)
        padding = 3
        self.min_x = min_x - padding
        self.max_x = max_x + padding
        self.min_y = min_y - padding
        self.max_y = max_y + padding

        self.grid_width = self.max_x - self.min_x + 1
        self.grid_height = self.max_y - self.min_y + 1

        canvas_width = self.grid_width * CELL_SIZE + 50  # extra space for axis labels
        canvas_height = self.grid_height * CELL_SIZE + 50

        self.canvas = tk.Canvas(root, width=canvas_width, height=canvas_height, bg="white")
        self.canvas.pack()

        # Controls
        controls = tk.Frame(root)
        controls.pack(pady=5)
        self.prev_btn = tk.Button(controls, text="Previous", command=self.prev_time)
        self.prev_btn.pack(side=tk.LEFT, padx=5)
        self.next_btn = tk.Button(controls, text="Next", command=self.next_time)
        self.next_btn.pack(side=tk.LEFT, padx=5)
        self.time_label = tk.Label(controls, text=f"Time: {self.current_time} / {self.T}")
        self.time_label.pack(side=tk.LEFT, padx=15)

        self.draw_grid()
        self.draw_scene()

    def precompute_log_positions(self):
        # For each log, for each time, store its points
        all_positions = []
        for log_idx, log in enumerate(self.logs):
            points = get_log_points(log)
            positions = [points]
            moves = self.directions[log_idx]
            for t in range(self.T):
                direction = moves[t] if t < len(moves) else ' '
                points = move_log_points(points, direction)
                positions.append(points)
            all_positions.append(positions)
        # Transpose: all_positions[log][time] -> want all_logs_at_time[time][log]
        all_logs_at_time = []
        for t in range(self.T + 1):
            all_logs_at_time.append([all_positions[log_idx][t] for log_idx in range(self.N)])
        return all_logs_at_time  # [time][log][points]

    def draw_grid(self):
        # Clear first
        self.canvas.delete("gridlines")
        self.canvas.delete("axis_labels")

        # Draw vertical grid lines + x axis labels
        for i in range(self.grid_width + 1):
            x = 50 + i * CELL_SIZE
            y_start = 50
            y_end = 50 + self.grid_height * CELL_SIZE
            self.canvas.create_line(x, y_start, x, y_end, fill="#aaa", width=GRID_LINE_WIDTH, tags="gridlines")

            # x axis label (coordinate), centered below each cell
            if i < self.grid_width:  # only label actual columns, not the last line
                coord = self.min_x + i
                label_x = x + CELL_SIZE // 2
                label_y = y_end + 20
                self.canvas.create_text(label_x, label_y, text=str(coord), tags="axis_labels", font=("Arial", 10))

        # Draw horizontal grid lines + y axis labels
        for j in range(self.grid_height + 1):
            y = 50 + j * CELL_SIZE
            x_start = 50
            x_end = 50 + self.grid_width * CELL_SIZE
            self.canvas.create_line(x_start, y, x_end, y, fill="#aaa", width=GRID_LINE_WIDTH, tags="gridlines")

            # y axis label (coordinate), centered left of each row
            if j < self.grid_height:  # only label actual rows, not the last line
                coord = self.max_y - j
                label_x = 30
                label_y = y + CELL_SIZE // 2
                self.canvas.create_text(label_x, label_y, text=str(coord), tags="axis_labels", font=("Arial", 10))

        # Draw axis labels titles
        self.canvas.create_text(50 + self.grid_width * CELL_SIZE / 2, 20, text="X Axis", font=("Arial", 12, "bold"), tags="axis_labels")
        self.canvas.create_text(20, 50 + self.grid_height * CELL_SIZE / 2, text="Y Axis", font=("Arial", 12, "bold"), angle=90, tags="axis_labels")

    def draw_scene(self):
        # Remove previous drawn cells
        self.canvas.delete("cells")

        # Draw Marian
        mx, my = self.marian_pos
        grid_x = mx - self.min_x
        grid_y = self.max_y - my  # invert y for drawing (top-left is 0,0)

        self.draw_cell(grid_x, grid_y, "M", "black", "yellow")

        # Draw logs positions, number them from 1 to N
        logs_points = self.all_log_positions[self.current_time]
        for idx, points in enumerate(logs_points):
            for (x, y) in points:
                gx = x - self.min_x
                gy = self.max_y - y
                if (gx, gy) == (grid_x, grid_y):
                    self.draw_cell(gx, gy, f"{idx + 1}", "white", "red")
                else:
                    self.draw_cell(gx, gy, f"{idx + 1}", "white", "blue")

        self.time_label.config(text=f"Time: {self.current_time} / {self.T}")

    def draw_cell(self, gx, gy, text, fg_color, bg_color):
        x1 = 50 + gx * CELL_SIZE
        y1 = 50 + gy * CELL_SIZE
        x2 = x1 + CELL_SIZE
        y2 = y1 + CELL_SIZE
        self.canvas.create_rectangle(x1, y1, x2, y2, fill=bg_color, tags="cells")
        self.canvas.create_text(x1 + CELL_SIZE / 2, y1 + CELL_SIZE / 2, text=text,
                                fill=fg_color, font=("Arial", 18, "bold"), tags="cells")

    def next_time(self):
        if self.current_time < self.T:
            self.current_time += 1
            self.draw_scene()

    def prev_time(self):
        if self.current_time > 0:
            self.current_time -= 1
            self.draw_scene()

if __name__ == "__main__":
    # Example input: 4 logs, each with its own movement string
    sample_input = """5 4
0 3
-2 0 -2 6
-4 4 0 4
-3 1 -3 4
-1 1 -1 4
NNNVV
VVSEE
EEVSS
VVEEE"""

    root = tk.Tk()
    root.title("Logs Movement Simulator")
    app = LogSimulatorGUI(root, sample_input)
    root.mainloop()