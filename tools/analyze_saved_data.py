import os
import json
import argparse

def analyze_results(root_folder):
    success_count = 0
    failure_count = 0
    pairs_with_nonzero_rc = []
    pairs_with_error_logs = []

    for symbol_folder in os.listdir(root_folder):
        if os.path.isdir(os.path.join(root_folder, symbol_folder)):
            return_code_path = os.path.join(root_folder, symbol_folder, "rc.txt")
            log_path = os.path.join(root_folder, symbol_folder, "terry.log")

            # Check if return code file exists
            if os.path.isfile(return_code_path):
                with open(return_code_path, "r") as return_code_file:
                    return_code = int(return_code_file.read())

                # Increment success or failure count
                if return_code == 0:
                    success_count += 1
                else:
                    failure_count += 1
                    pairs_with_nonzero_rc.append(symbol_folder)

            # Check for [error] logs in terry.log
            if os.path.isfile(log_path):
                with open(log_path, "r") as log_file:
                    log_content = log_file.read()

                if "[error]" in log_content.lower():
                    pairs_with_error_logs.append(symbol_folder)

    # Print statistics
    print(f"Total successful cases: {success_count}")
    print(f"Total failed cases: {failure_count}")

    # Print pairs with non-zero return code
    if pairs_with_nonzero_rc:
        print("\nPairs with non-zero return code:")
        for pair in pairs_with_nonzero_rc:
            print(pair)

    # Print pairs with error logs
    if pairs_with_error_logs:
        print("\nPairs with at least one [error] log:")
        for pair in pairs_with_error_logs:
            print(pair)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Analyze saved data results.')
    parser.add_argument('--config', type=str, default='config.json', help='Path to the config file')
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    config_path = os.path.join(script_dir, args.config)

    with open(config_path, "r") as config_file:
        config_data = json.load(config_file)
    analyze_results(config_data["folder_path"])
