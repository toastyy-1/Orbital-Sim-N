import struct
import os
import matplotlib.pyplot as plt

data_debug = False

####################################################################################################
# BINARY FILE READING
####################################################################################################
struct_format = 'didd'  # double int double double
struct_size = struct.calcsize(struct_format)

script_dir = os.path.dirname(os.path.abspath(__file__))
filename = os.path.join(script_dir, "body_pos_data.bin")
file_size = os.path.getsize(filename)

num_records = file_size // struct_size
print(f"Data read from {num_records} records")

# read binary file - returns separate arrays for each data element
def readBodyPos(filename):
    ts = []
    idx = []
    xpos = []
    ypos = []

    with open(filename, "rb") as file:
        while True:
            data = file.read(struct_size)
            if len(data) < struct_size:
                break
            t, body_index, x, y = struct.unpack(struct_format, data)
            ts.append(t)
            idx.append(body_index)
            xpos.append(x)
            ypos.append(y)

    return ts, idx, xpos, ypos

timestamps, body_indices, x_positions, y_positions = readBodyPos(filename)

if data_debug:
    for i in range(len(timestamps)):
        print(f"Body {body_indices[i]}: ({x_positions[i]:.3f}, {y_positions[i]:.3f}) at t={timestamps[i]:.3f}")

####################################################################################################
# PLOTTING
####################################################################################################
# get unique body indices
unique_body_indices = set(body_indices)

# create figure with subplots
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 14))

# plot for each body
for body_idx in sorted(unique_body_indices):
    # get x, y positions and timestamps for this body
    body_timestamps = []
    body_x_positions = []
    body_y_positions = []
    for i in range(len(body_indices)):
        if body_indices[i] == body_idx:
            body_timestamps.append(timestamps[i])
            body_x_positions.append(x_positions[i])
            body_y_positions.append(y_positions[i])

    # subplot 1: orbital trajectories (x vs y)
    ax1.plot(body_x_positions, body_y_positions, label=f'Body {body_idx}', marker='o', markersize=0.1)

    # subplot 2: x position vs time
    ax2.plot(body_timestamps, body_x_positions, label=f'Body {body_idx}', marker='o', markersize=0.1)

    # subplot 3: y position vs time
    ax3.plot(body_timestamps, body_y_positions, label=f'Body {body_idx}', marker='o', markersize=0.1)

# configure subplot 1 (orbital trajectories)
ax1.set_xlabel('X Position')
ax1.set_ylabel('Y Position')
ax1.set_title('Orbital Trajectories')
ax1.legend()
ax1.grid(True)
ax1.axis('equal')

# configure subplot 2 (x vs time)
ax2.set_xlabel('Time')
ax2.set_ylabel('X Position')
ax2.set_title('X Position vs Time')
ax2.grid(True)

# configure subplot 3 (y vs time)
ax3.set_xlabel('Time')
ax3.set_ylabel('Y Position')
ax3.set_title('Y Position vs Time')
ax3.grid(True)

plt.tight_layout()
plt.show()
