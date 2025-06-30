import os
import subprocess
import time
from concurrent.futures import ThreadPoolExecutor
import json
import argparse
from datetime import timedelta

def run_terry(symbol, folder_path, terry_path, timer, max_retries=2):
    # Get the current date and time in the Unix timestamp format
    start_time = int(time.time())

    # Prepare folder name
    timestamp = time.strftime("%Y%m%d")
    symbol_folder_name = f"{symbol}_{timestamp}_{start_time}"
    symbol_folder = os.path.join(folder_path, symbol_folder_name)

    # Create a folder for the current symbol
    os.makedirs(symbol_folder, exist_ok=True)

    log_filename = "terry.log"
    log_path = os.path.join(symbol_folder, log_filename)
    return_code_path = os.path.join(symbol_folder, "rc.txt")

    command = [terry_path, 'stream', 'save', f'--symbol={symbol}', f'--save-path={symbol_folder}', f'--timer={timer}']

    for retry in range(max_retries + 1):

        # Run terry.exe for the current symbol
        with open(log_path, "w") as log_file:
            process = subprocess.Popen(command, stdout=log_file, stderr=log_file, text=True, creationflags=subprocess.CREATE_NO_WINDOW)

        status = f"streamsaver for {symbol} running... (Retry {retry})"
        print(status)

        # Wait for the process to finish
        return_code = process.wait()

        # Write the return code to a file
        with open(return_code_path, "w") as return_code_file:
            return_code_file.write(str(return_code))

        # Determine the status based on the return code
        if return_code == 0:
            status = f"streamsaver for {symbol} finished"
            print(status)
            break  # Break the retry loop if successful
        else:
            status = f"{symbol} failed (Retry {retry})"
            print(status)

    # Print a final status after retries
    if return_code != 0:
        print(f"Maximum retries reached for {symbol}. Giving up.")

def run_timer(timer):
    start_time = time.time()

    try:
        while True:
            elapsed_time = int(time.time() - start_time)
            remaining_time = max(timer - elapsed_time, 0)
            formatted_remaining_time = str(timedelta(seconds=remaining_time))
            print(f"Time remaining: {formatted_remaining_time}", end="\r")
            time.sleep(1)
            # Exit the loop if the timer is reached
            if elapsed_time >= timer:
                break
    except KeyboardInterrupt:
        pass  # Handle Ctrl+C to exit gracefully

def main():
    # Set an environment variable
    os.environ["SPDLOG_LEVEL"] = "info"

    parser = argparse.ArgumentParser(description='Run terry.exe with specified options')
    parser.add_argument('--config', type=str, default='config.json', help='Path to the config file')
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    config_path = os.path.join(script_dir, args.config)

    with open(config_path, "r") as config_file:
        config_data = json.load(config_file)

    # Set the maximum number of threads to the number of symbols in the config
    max_workers = len(config_data["symbols"])

    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        # Run processes asynchronously
        futures = []
        for symbol in config_data["symbols"]:
            future = executor.submit(run_terry, symbol, config_data["folder_path"], config_data["terry_path"], config_data["timer"])
            time.sleep(3)
            futures.append(future)

        run_timer(config_data["timer"])

        # Wait for the processes to complete and print the status once available
        for future in futures:
            future.result()

if __name__ == "__main__":
    main()
