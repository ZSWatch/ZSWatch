import pylink
import argparse
import sys
import time
import os
from struct import *
import imufusion
import matplotlib.pyplot as pyplot
import numpy as np
import sys
from threading import Thread
from datetime import datetime

logged_data = []

num_discard = 5


def run_fusion(data_list):
    sample_rate = 100  # 100 Hz

    timestamp = data_list[:, 0]
    euler_from_watch = data_list[:, 1:4]
    gyroscope = data_list[:, 4:7]
    accelerometer = data_list[:, 7:10]
    magnetometer = data_list[:, 10:13]

    max_x = np.max(magnetometer[:, 0])
    max_y = np.max(magnetometer[:, 1])
    max_z = np.max(magnetometer[:, 2])

    min_x = np.min(magnetometer[:, 0])
    min_y = np.min(magnetometer[:, 1])
    min_z = np.min(magnetometer[:, 2])

    offset_x = (max_x + min_x) / 2
    offset_y = (max_y + min_y) / 2
    offset_z = (max_z + min_z) / 2

    print("Offset x: ", offset_x)
    print("Offset y: ", offset_y)
    print("Offset z: ", offset_z)

    # Uncomment and possibly hardcode if you have collected calibration data
    # and then want to use it to try the calibration on new data
    # magnetometer[:, 0] -= offset_x
    # magnetometer[:, 1] -= offset_y
    # magnetometer[:, 2] -= offset_z

    # Instantiate algorithms
    # offset = imufusion.Offset(sample_rate)
    ahrs = imufusion.Ahrs()

    ahrs.settings = imufusion.Settings(
        imufusion.CONVENTION_NWU,  # convention
        0.5,  # gain
        2000,  # gyroscope range
        10,  # acceleration rejection
        10,  # magnetic rejection
        5 * sample_rate,
    )  # recovery trigger period = 5 seconds

    # Process sensor data
    timestamp = timestamp - timestamp[0]
    delta_time = np.diff(timestamp, prepend=timestamp[0])

    euler = np.empty((len(timestamp), 3))
    internal_states = np.empty((len(timestamp), 6))
    flags = np.empty((len(timestamp), 4))

    for index in range(len(timestamp)):
        # gyroscope[index] = offset.update(gyroscope[index])

        ahrs.update_no_magnetometer(
            gyroscope[index], accelerometer[index], delta_time[index]
        )
        # ahrs.update(gyroscope[index], accelerometer[index], magnetometer[index], delta_time[index])

        euler[index] = ahrs.quaternion.to_euler()

        ahrs_internal_states = ahrs.internal_states
        internal_states[index] = np.array(
            [
                ahrs_internal_states.acceleration_error,
                ahrs_internal_states.accelerometer_ignored,
                ahrs_internal_states.acceleration_recovery_trigger,
                ahrs_internal_states.magnetic_error,
                ahrs_internal_states.magnetometer_ignored,
                ahrs_internal_states.magnetic_recovery_trigger,
            ]
        )

        ahrs_flags = ahrs.flags
        flags[index] = np.array(
            [
                ahrs_flags.initialising,
                ahrs_flags.angular_rate_recovery,
                ahrs_flags.acceleration_recovery,
                ahrs_flags.magnetic_recovery,
            ]
        )

    mag_x = (magnetometer[:, 0],)
    mag_y = (magnetometer[:, 1],)
    mag_z = (magnetometer[:, 2],)

    fig, ax = pyplot.subplots(1, 1)
    pyplot.title("Magnetometer data")
    ax.set_aspect(1)
    ax.scatter(mag_x, mag_y, color="r")
    ax.scatter(mag_y, mag_z, color="g")
    ax.scatter(mag_z, mag_x, color="b")

    # Plot Euler angles
    figure, axes = pyplot.subplots(
        nrows=13,
        sharex=True,
        gridspec_kw={"height_ratios": [5, 5, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 5]},
    )

    figure.suptitle("Euler angles, internal states, and flags")

    axes[0].plot(timestamp, euler[:, 0], "tab:orange", label="Roll")
    axes[0].plot(timestamp, euler[:, 1], "tab:green", label="Pitch")
    axes[0].plot(timestamp, euler[:, 2], "tab:blue", label="Yaw")
    axes[0].set_ylabel("Degrees")
    axes[0].grid()
    axes[0].legend()
    axes[0].set_ylim(-180, 180)

    axes[1].plot(timestamp, euler_from_watch[:, 0], "tab:orange", label="Roll")
    axes[1].plot(timestamp, euler_from_watch[:, 1], "tab:green", label="Pitch")
    axes[1].plot(timestamp, euler_from_watch[:, 2], "tab:blue", label="Yaw")
    axes[1].set_ylabel("Degrees")
    axes[1].grid()
    axes[1].legend()
    axes[1].set_ylim(-180, 180)

    # Plot initialising flag
    plot_bool(axes[2], timestamp, flags[:, 0], "Initialising")

    # Plot angular rate recovery flag
    plot_bool(axes[3], timestamp, flags[:, 1], "Angular rate recovery")

    # Plot acceleration rejection internal states and flags
    axes[4].plot(
        timestamp, internal_states[:, 0], "tab:olive", label="Acceleration error"
    )
    axes[4].set_ylabel("Degrees")
    axes[4].grid()
    axes[4].legend()

    plot_bool(axes[5], timestamp, internal_states[:, 1], "Accelerometer ignored")

    axes[6].plot(
        timestamp,
        internal_states[:, 2],
        "tab:orange",
        label="Acceleration recovery trigger",
    )
    axes[6].grid()
    axes[6].legend()

    plot_bool(axes[7], timestamp, flags[:, 2], "Acceleration recovery")

    # Plot magnetic rejection internal states and flags
    axes[8].plot(timestamp, internal_states[:, 3], "tab:olive", label="Magnetic error")
    axes[8].set_ylabel("Degrees")
    axes[8].grid()
    axes[8].legend()

    plot_bool(axes[9], timestamp, internal_states[:, 4], "Magnetometer ignored")

    axes[10].plot(
        timestamp,
        internal_states[:, 5],
        "tab:orange",
        label="Magnetic recovery trigger",
    )
    axes[10].grid()
    axes[10].legend()

    plot_bool(axes[11], timestamp, flags[:, 3], "Magnetic recovery")

    axes[12].plot(timestamp, magnetometer[:, 0], "tab:orange", label="Roll")
    axes[12].plot(timestamp, magnetometer[:, 1], "tab:green", label="Pitch")
    axes[12].plot(timestamp, magnetometer[:, 2], "tab:blue", label="Yaw")
    axes[12].set_ylabel("Degrees")
    axes[12].grid()
    axes[12].legend()

    pyplot.show(block="no_block" not in sys.argv)  # don't block when script run by CI


def plot_bool(axis, x, y, label):
    axis.plot(x, y, "tab:cyan", label=label)
    pyplot.sca(axis)
    pyplot.yticks([0, 1], ["False", "True"])
    axis.grid()
    axis.legend()


def read_from_rtt_log(jlink, num_samples):
    try:
        while jlink.connected() and len(logged_data) < num_samples:
            data = jlink.rtt_read(1, 4096 * 2)
            if len(data) == 0:
                continue
            data = bytes(data)
            data.decode("utf-8").split("\n")
            for line in data.decode("utf-8").split("\n"):
                if len(line) > 0:
                    print(len(logged_data), line)
                    logged_data.append(line.split(","))

    except Exception:
        print("IO read thread exception, exiting...")
        raise


def rtt_run_read(target_device, num_samples):
    jlink = pylink.JLink()
    print("Connecting to JLink...")
    jlink.open()
    print("Connecting to %s..." % target_device)
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
    jlink.connect(target_device)
    jlink.rtt_start(None)
    print("Connected")

    while True:
        try:
            num_up = jlink.rtt_get_num_up_buffers()
            num_down = jlink.rtt_get_num_down_buffers()
            print("RTT started, %d up bufs, %d down bufs." % (num_up, num_down))
            break
        except pylink.errors.JLinkRTTException:
            time.sleep(0.1)

    print("up channels:")
    for buf_index in range(jlink.rtt_get_num_up_buffers()):
        buf = jlink.rtt_get_buf_descriptor(buf_index, True)
        print(
            "    %d: name = %r, size = %d bytes, flags = %d"
            % (buf.BufferIndex, buf.name, buf.SizeOfBuffer, buf.Flags)
        )

    print("down channels:")
    for buf_index in range(jlink.rtt_get_num_down_buffers()):
        buf = jlink.rtt_get_buf_descriptor(buf_index, False)
        print(
            "    %d: name = %r, size = %d bytes, flags = %d"
            % (buf.BufferIndex, buf.name, buf.SizeOfBuffer, buf.Flags)
        )

    try:
        work_thread = Thread(
            target=read_from_rtt_log, args=(jlink, num_samples + num_discard)
        )
        work_thread.daemon = True
        work_thread.start()
        work_thread.join()
        jlink.close()
        print("JLink disconnected, running fusion...")

        data_list = np.array(
            logged_data[num_discard:]
        )  # Skip the first x lines of the log as they sometimes are old data
        data_list = np.char.strip(data_list)
        data_list = data_list.astype(np.float64)

        now = datetime.now()
        date_time = now.strftime("%d_%m_%Y-%H-%M")
        np.savetxt("collected_{}.csv".format(date_time), data_list, delimiter=",")

        run_fusion(data_list)

    except KeyboardInterrupt:
        print("ctrl-c detected, exiting...")
        jlink.close()
        pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Open RTT console.")
    parser.add_argument(
        "--target_cpu",
        default="nRF5340_XXAA",
        help="Device Name (see https://www.segger.com/supported-devices/jlink/)",
    )

    parser.add_argument(
        "--samples",
        type=int,
        default=500,
        help="Number of samples to collect before running fusion.",
    )

    parser.add_argument(
        "--data", type=str, help="Don't collect data, use data from file instead."
    )

    args = parser.parse_args()

    if args.data:
        data_list = np.genfromtxt(args.data, delimiter=",")
        run_fusion(data_list)
        sys.exit(0)

    sys.exit(rtt_run_read(args.target_cpu, args.samples))
