import pylink
import argparse
import sys
import time
import subprocess
from struct import *
import matplotlib.pyplot as pyplot
import numpy as np
import sys
from threading import Thread
from datetime import datetime
import wave

logged_pcm_data = bytearray()
num_discard = 5


def read_from_rtt_log(jlink):
    global logged_pcm_data
    try:
        no_data_timeout = 2.0  # 2 second timeout if no new data
        last_data_time = time.time()
        data_received = False

        while jlink.connected():
            data = jlink.rtt_read(2, 4096)
            current_time = time.time()

            if len(data) == 0:
                # No data efter some time, we treat this as recording finished
                if data_received and current_time - last_data_time > no_data_timeout:
                    print(
                        f"No new data for {no_data_timeout}s, assuming recording finished"
                    )
                    break
                continue

            data_received = True
            last_data_time = current_time
            logged_pcm_data += bytearray(data)
            print("Got", len(data), "Total:", len(logged_pcm_data))

        if data_received:
            print(f"Data collection finished: {len(logged_pcm_data)} bytes collected")
    except Exception:
        print("IO read thread exception, exiting...")
        raise


def rtt_run_read(target_device):
    jlink = pylink.JLink()
    print("Connecting to JLink...")
    jlink.open()
    print("Connecting to %s..." % target_device)
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
    jlink.connect(target_device)
    jlink.rtt_start(None)
    time.sleep(2)
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

    session_count = 0

    try:
        while True:
            session_count += 1
            print(f"\n{'='*60}")
            print(f"SESSION {session_count} - Waiting for audio data...")
            print(f"{'='*60}")

            # Reset data for new session
            global logged_pcm_data
            logged_pcm_data = bytearray()

            work_thread = Thread(target=read_from_rtt_log, args=(jlink,))
            work_thread.daemon = True
            work_thread.start()
            work_thread.join()

            if len(logged_pcm_data) > 16000:  # Only process if we got meaningful data
                print(f"\nProcessing {len(logged_pcm_data)} bytes of audio data...")

                now = datetime.now()
                date_time = now.strftime("%d_%m_%Y-%H-%M-%S")
                session_suffix = f"_{session_count:03d}"

                # Save raw PCM data
                pcm_filename = f"pcm_data_{date_time}{session_suffix}.raw"
                with open(pcm_filename, "wb") as f:
                    f.write(logged_pcm_data)
                print(f"Saved raw PCM: {pcm_filename}")

                # Convert to WAV and play
                wav_filename = f"converted_pcm_{date_time}{session_suffix}.wav"
                create_wav(logged_pcm_data[16000:], wav_filename)
                print(f"Converted to WAV: {wav_filename}")

                if play_audio(wav_filename):
                    print("Playback completed successfully")
                else:
                    print("Playback failed")

                print(f"Ready for next recording session...")
                # Small delay before next session only if we got data
                time.sleep(1)
            else:
                # Don't increment session count, just keep waiting silently
                session_count -= 1

    except KeyboardInterrupt:
        print("\nCtrl-C detected, closing JLink and exiting...")
        jlink.close()
        print("JLink disconnected")
        return 0


def create_wav(raw_pcm_data, name):
    # Configure the WAV file settings
    num_channels = 1
    sample_width = 2  # 16-bit audio
    frame_rate = 16000  # 16kHz
    compression_type = "NONE"
    compression_name = "NONE"

    processed_data = bytearray()
    for i in range(0, len(raw_pcm_data), 2):
        if i + 1 < len(raw_pcm_data):
            low_byte = raw_pcm_data[i]
            high_byte = raw_pcm_data[i + 1]
            processed_data.append(high_byte)
            processed_data.append(low_byte)

    num_frames = len(processed_data) // 2

    with wave.open(name, "wb") as wavfile:
        wavfile.setparams(
            (
                num_channels,
                sample_width,
                frame_rate,
                num_frames,
                compression_type,
                compression_name,
            )
        )
        wavfile.writeframes(processed_data)


def play_audio(wav_filename):
    """Play the WAV file using aplay (Linux)"""
    try:
        # Try aplay first (common on Linux)
        print(f"Playing audio file: {wav_filename}")
        result = subprocess.run(
            ["aplay", wav_filename], capture_output=True, text=True, timeout=30
        )
        if result.returncode == 0:
            print("Audio playback completed successfully")
            return True
    except (subprocess.TimeoutExpired, FileNotFoundError):
        pass
    return False


def main():
    parser = argparse.ArgumentParser(description="Mic data collection.")
    parser.add_argument(
        "--target_cpu",
        default="nRF5340_XXAA",
        help="Device Name (see https://www.segger.com/supported-devices/jlink/)",
    )

    args = parser.parse_args()

    return rtt_run_read(args.target_cpu)


if __name__ == "__main__":
    sys.exit(main())
