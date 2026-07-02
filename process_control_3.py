import cv2
import requests
import time
from ultralytics import YOLO

# ==============================
# CONFIG
# ==============================
STREAM_URL = "http://192.168.1.23:81/stream"
CONTROL_URL = "http://192.168.1.21/cmd?val="

model = YOLO("yolov8n.pt")

cap = cv2.VideoCapture(STREAM_URL, cv2.CAP_FFMPEG)

FRAME_WIDTH = 320
FRAME_HEIGHT = 240

CENTER_THRESHOLD = 60
MIN_AREA = 12000
MAX_AREA = 20000

last_cmd = ""
last_time = 0
CMD_DELAY = 0.5  # VERY IMPORTANT 

frame_count = 0


# ==============================
# CONTROLLED COMMAND SENDER
# ==============================
def send_cmd(cmd):
    global last_cmd, last_time

    # rate limiting
    if cmd == last_cmd and (time.time() - last_time) < CMD_DELAY:
        return

    try:
        requests.get(CONTROL_URL + cmd, timeout=0.2)
        print(f"CMD → {cmd}")
        last_cmd = cmd
        last_time = time.time()
    except:
        print("ESP32 not reachable")


# ==============================
# MAIN LOOP
# ==============================
while True:
    cap.grab()
    ret, frame = cap.read()

    if not ret:
        print("⚠️ Stream lost... reconnecting")
        send_cmd("S")  
        cap.release()
        time.sleep(2)
        cap = cv2.VideoCapture(STREAM_URL, cv2.CAP_FFMPEG)
        continue
    frame = cv2.resize(frame, (FRAME_WIDTH, FRAME_HEIGHT))
    frame_center = FRAME_WIDTH // 2

    # ==============================
    # FRAME SKIP (IMPORTANT)
    # ==============================
    frame_count += 3
    if frame_count % 9 != 0:   # reduce load MORE
        cv2.imshow("Human Following AI", frame)
        if cv2.waitKey(1) == 27:
            break
        continue

    # ==============================
    # YOLO
    # ==============================
    results = model(frame, verbose=False)

    persons = []

    for r in results:
        for box in r.boxes:
            if int(box.cls[0]) == 0:
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                persons.append((x1, y1, x2, y2))

    # ==============================
    # DECISION
    # ==============================
    if persons:
        target = max(persons, key=lambda b: (b[2]-b[0])*(b[3]-b[1]))

        x1, y1, x2, y2 = target
        cx = (x1 + x2) // 2
        area = (x2 - x1) * (y2 - y1)

        cv2.rectangle(frame, (x1,y1), (x2,y2), (0,255,0), 2)
        cv2.line(frame, (frame_center, 0), (frame_center, FRAME_HEIGHT), (255,0,0), 2)

        if cx < frame_center - CENTER_THRESHOLD:
            cmd = "L"
        elif cx > frame_center + CENTER_THRESHOLD:
            cmd = "R"
        else:
            if area < MIN_AREA:
                cmd = "F"
                print(area)
            elif area > MAX_AREA:
                cmd = "B"
                print(area)
            else:
                cmd = "S"

        send_cmd(cmd)

        cv2.putText(frame, f"CMD: {cmd}", (10,20),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,255), 2)
    else:
        send_cmd("S")

    cv2.imshow("Human Following AI", frame)

    if cv2.waitKey(1) == 27:
        break


cap.release()
cv2.destroyAllWindows()