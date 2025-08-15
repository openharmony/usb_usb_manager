import subprocess
import re
import time
import cv2
import os

# ANSI escape codes for colors
RED = "\033[91m"
GREEN = "\033[92m"
RESET = "\033[0m"

def run_command(command):
    """Run a system command using subprocess."""
    try:
        result = subprocess.run(command, shell=True, check=True, capture_output=True, text=True)
        print(result.stdout)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running command '{command}': {e.stderr}")
        raise

def install_hap(hap_file):
    """Install a HAP file using hdc."""
    print(f"Installing {hap_file}...")
    run_command(f"hdc install {hap_file}")

def run_hap(ability, bundleName):
    """Run a HAP using hdc."""
    print(f"Running {bundleName} with ability {ability}...")
    run_command(f"hdc shell aa start -a {ability} -b {bundleName}")

def delete_existing_snapshots():
    """Delete existing snapshots on the device."""
    print("Deleting existing snapshots...")
    run_command("hdc shell rm -f /data/local/tmp/snapshot_*.jpeg")

def capture_screenshot():
    """Capture a screenshot from the device and return the local file path."""
    delete_existing_snapshots()

    output = run_command("hdc shell snapshot_display")
    match = re.search(r'/data/local/tmp/snapshot_\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2}\.jpeg', output)
    
    if not match:
        raise ValueError("Failed to find the snapshot file path in the output")

    remote_file_path = match.group(0)
    local_file_path = "snapshot.jpeg"
    run_command(f"hdc file recv {remote_file_path} {local_file_path}")
    
    return local_file_path

def find_button(template_path, screenshot_path):
    """Find the button in the screenshot using OpenCV template matching."""
    screenshot = cv2.imread(screenshot_path)
    template = cv2.imread(template_path)

    result = cv2.matchTemplate(screenshot, template, cv2.TM_CCOEFF_NORMED)
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(result)

    # Determine the coordinates of the center of the matched region
    template_height, template_width = template.shape[:2]
    center_x = max_loc[0] + template_width // 2
    center_y = max_loc[1] + template_height // 2

    return center_x, center_y

def tap_screen(x, y):
    """Simulate a tap on the screen at the given coordinates using uinput."""
    print(f"Tapping on screen at coordinates ({x}, {y})...")
    run_command(f"hdc shell uinput -T -d {x} {y}")
    run_command(f"hdc shell uinput -T -u {x} {y}")

def retrieve_and_combine_logs(log1_remote_path, log2_remote_path, combined_log_path):
    """Retrieve two log files from the device and combine them into a single file."""
    log1_local_path = "usb_info.log"
    log2_local_path = "usb_automation.log"

    # Retrieve the log files from the device
    run_command(f"hdc file recv {log1_remote_path} {log1_local_path}")
    run_command(f"hdc file recv {log2_remote_path} {log2_local_path}")

    # Combine the log files
    with open(combined_log_path, 'w', encoding='utf-8') as combined_log:
        for log_path in [log1_local_path, log2_local_path]:
            with open(log_path, 'r', encoding='utf-8', errors='replace') as log_file:
                combined_log.write(log_file.read())
                combined_log.write("\n")

    print(f"Combined log file saved to {combined_log_path}")

    # Delete individual log files after combining
    os.remove(log1_local_path)
    os.remove(log2_local_path)
    print("Individual log files deleted.\n")

def analyze_logs(combined_log_path):
    """Analyze the combined log file for occurrences of '失败' and extract relevant lines."""
    keyword1 = "失败"
    keyword2 = "没有"
    extracted_lines = []

    with open(combined_log_path, 'r', encoding='utf-8', errors='replace') as log_file:
        for line in log_file:
            if keyword1 in line or keyword2 in line:
                extracted_lines.append(line.strip())

    if extracted_lines:
        print(f"{RED}Extracted failed tests:{RESET}")
        for line in extracted_lines:
            print(f"{RED}{line}{RESET}")
    else:
        print(f"{GREEN}Congratulations! All tests passed.{RESET}")

def main():
    hap1 = "usbInfo-default-signed.hap"
    hap1BundleName = "com.example.nomralapp"
    hap1Ability = "UsbInfoAbility"

    hap2 = "usb_fullAutomation_newsigned.hap"
    hap2BundleName = "com.example.automationapp"
    hap2Ability = "UsbAutomationAbility"

    template_path = "allow_button_template.jpeg" 

    log1_remote_path = "/data/app/el2/100/base/com.example.nomralapp/haps/usbInfo/files/usb_info.log"
    log2_remote_path = "/data/app/el2/100/base/com.example.automationapp/haps/usbAutomation/files/usb_automation.log"
    combined_log_path = "combined_log.log"

    try:
        install_hap(hap1)
        install_hap(hap2)

        run_hap(hap1Ability, hap1BundleName)
        
        time.sleep(3) 

        screenshot_path = capture_screenshot()

        x, y = find_button(template_path, screenshot_path)

        tap_screen(x, y)

        run_hap(hap2Ability, hap2BundleName)

        time.sleep(8)

        # Retrieve and combine the log files
        retrieve_and_combine_logs(log1_remote_path, log2_remote_path, combined_log_path)

        time.sleep(1)
        # Analyze the combined log file
        analyze_logs(combined_log_path)

        input("\nFor more test information, please refer to combined_log.txt. Press Enter to exit...")
        
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    main()