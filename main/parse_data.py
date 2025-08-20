import numpy as np
import matplotlib.pyplot as plt
import math

def parse_encoder_line(line):
    # remove leading/trailing whitespace
    line = line.strip()
    if len(line) != 23:
        raise ValueError(f"Unexpected line length: {line}")
    # get seconds and ms
    sec = line[0:4]
    if sec == "OVER":
        raise ValueError(f"Seconds exceeds max expected value")
    else:
        sec = int(sec)
    ms = line[4:7]
    if ms == "OVR":
        raise ValueError(f"Milliseconds exceeds max expected value")
    else:
        ms = int(ms)
    time = sec + ms / 1000

    # get right encoder val
    if line[7:15] == "OVERMAXI": # handle unexpected val
        raise ValueError(f"Right encoder exceeds max expected value")
    else: 
        r_sign = -1 if line[7] == '1' else 1 # changed
        r_str = line[8:15]
        r_val = r_sign * float(r_str)

    # get left encoder val
    if line[15:23] == "OVERMAXI": # handle unexpected val
        raise ValueError(f"Left encoder exceeds max expected value")
    else: 
        l_sign = -1 if line[15] == '1' else 1 #changed 
        l_str = line[16:23]
        l_val = l_sign * float(l_str)

    return time, r_val, l_val

def process_file(file_path):
    data = []
    
    with open(file_path, 'r') as f:
        for line in f:
            try:
                time, r_val, l_val = parse_encoder_line(line)
                data.append([time, r_val, l_val])
            except ValueError as e:
                print(f"Skipping line due to error: {e}")
    
    # Convert to NumPy array and transpose to get shape (3, N)
    data_array = np.array(data).T
    return data_array

# wheel_dist is the distance between the wheels
def compute_odometry(r_dists, l_dists, wheel_dist):
    x, y, theta = 0.0, 0.0, 0.0
    coords = [(x, y)]

    for i in range(1, len(l_dists)):
        # wheel displacements
        dl = l_dists[i] - l_dists[i - 1]
        dr = r_dists[i] - r_dists[i - 1]

        d = (dl + dr) / 2.0 # approx displacement of robot
        dTheta = (dr - dl) / wheel_dist # approx angular displacement

        # update xy coordinates and theta
        x += d * np.cos(theta + (dTheta / 2.0))
        y += d * np.sin(theta + (dTheta / 2.0))
        theta += dTheta

        coords.append((x, y))
    
    return np.array(coords)

def rot_to_dist(rotations, wheel_circumference):
    return np.array(rotations) * wheel_circumference

def main():
    # variables for robot dimensions, in cm right now
    # subject to change
    wheel_circumference = math.pi * 6.0 # 6 cm diameter 
    center_to_wheel = 4.5 # 4.5 mm
    wheel_dist = center_to_wheel * 2.0

    file_path = "/Users/miamoto/Documents/RR_data/encoder/encoder_highfreq.txt"  # Change this to your actual file path
    data_array = process_file(file_path)
    print(data_array)
    print(f"Data array size: {data_array.shape}")  # Should be (3, N)

    # Unpack the rows of the 3×N array
    time, right_vals, left_vals = data_array

    # convert to distance traveled by each wheel
    r_dists = rot_to_dist(right_vals, wheel_circumference)
    l_dists = rot_to_dist(left_vals, wheel_circumference)

    coords = compute_odometry(r_dists, l_dists, wheel_dist)

    plt.figure(figsize=(8, 6))
    plt.plot(coords[:, 0], coords[:, 1], marker='o', markersize=4, label='Robot Path')
    plt.plot(0, 0, 'ro', label='Origin')  # 'ro' = red circle
    plt.plot(coords[1, 0], coords[1, 1], 'go', label='Second point')
    plt.xlabel('X Position (cm)')
    plt.ylabel('Y Position (cm)')
    plt.title('Robot Path via Odometry')
    plt.grid(True)
    plt.axis('equal')
    plt.legend()
    plt.show()

if __name__ == "__main__":
    main()

# def get_gyro_data(data):
#     if len(data) != 9:
#         raise ValueError(f"Unexpected gyro data length: {data}")
    
#     return

# def parse_imu_line(line):
#     # remove leading/trailing whitespace
#     line = line.strip()
#     if len(line) != 88:
#         raise ValueError(f"Unexpected line length: {line}")
#     # get seconds and ms
#     sec = line[0:4]
#     if sec == "OVER":
#         raise ValueError(f"Seconds exceeds max expected value")
#     else:
#         sec = int(sec)
#     ms = line[4:7]
#     if ms == "OVR":
#         raise ValueError(f"Milliseconds exceeds max expected value")
#     else:
#         ms = int(ms)

#     return