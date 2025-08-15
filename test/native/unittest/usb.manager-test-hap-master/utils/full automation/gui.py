import subprocess
import re
import sys
import time
import cv2
import os
import threading
import tkinter as tk
from tkinter import scrolledtext
from tkinter import ttk

def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    try:
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)


def run_command(command, output_widget):
    """Run a system command using subprocess and update the output widget."""
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    full_output = ""
    while True:
        output = process.stdout.readline()
        if output:
            output_widget.insert(tk.END, output)
            output_widget.see(tk.END)
            full_output += output
        if process.poll() is not None:
            break
    return full_output

def install_hap(hap_file, output_widget):
    """Install a HAP file using hdc."""
    return run_command(f"hdc install {hap_file}", output_widget)

def run_hap(ability, bundleName, output_widget):
    """Run a HAP using hdc."""
    return run_command(f"hdc shell aa start -a {ability} -b {bundleName}", output_widget)

def delete_existing_snapshots(output_widget):
    """Delete existing snapshots on the device."""
    return run_command("hdc shell rm -f /data/local/tmp/snapshot_*.jpeg", output_widget)

def capture_screenshot(output_widget):
    """Capture a screenshot from the device and return the local file path."""
    delete_existing_snapshots(output_widget)

    output = run_command("hdc shell snapshot_display", output_widget)
    time.sleep(2)
    match = re.search(r'/data/local/tmp/snapshot_\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2}\.jpeg', output)
    
    if not match:
        raise ValueError("Failed to find the snapshot file path in the output")

    remote_file_path = match.group(0)
    local_file_path = "snapshot.jpeg"
    run_command(f"hdc file recv {remote_file_path} {local_file_path}", output_widget)
    
    return local_file_path

def find_button(template_path, screenshot_path):
    """Find the button in the screenshot using OpenCV template matching."""
    screenshot = cv2.imread(screenshot_path)
    template = cv2.imread(template_path)

    result = cv2.matchTemplate(screenshot, template, cv2.TM_CCOEFF_NORMED)
    _, _, _, max_loc = cv2.minMaxLoc(result)

    template_height, template_width = template.shape[:2]
    center_x = max_loc[0] + template_width // 2
    center_y = max_loc[1] + template_height // 2

    return center_x, center_y

def tap_screen(x, y, output_widget):
    """Simulate a tap on the screen at the given coordinates using uinput."""
    run_command(f"hdc shell uinput -T -d {x} {y}", output_widget)
    run_command(f"hdc shell uinput -T -u {x} {y}", output_widget)

def retrieve_and_combine_logs(log1_remote_path, log2_remote_path, combined_log_path, output_widget):
    """Retrieve two log files from the device and combine them into a single file."""
    log1_local_path = "usb_info.log"
    log2_local_path = "usb_automation.log"

    run_command(f"hdc file recv {log1_remote_path} {log1_local_path}", output_widget)
    run_command(f"hdc file recv {log2_remote_path} {log2_local_path}", output_widget)

    with open(combined_log_path, 'w', encoding='utf-8') as combined_log:
        for log_path in [log1_local_path, log2_local_path]:
            with open(log_path, 'r', encoding='utf-8', errors='replace') as log_file:
                combined_log.write(log_file.read())
                combined_log.write("\n")

    os.remove(log1_local_path)
    os.remove(log2_local_path)

def analyze_logs(combined_log_path, log_output_widget, failure_count_widget, failed_tests_widget):
    """Analyze the combined log file for occurrences of '失败' and extract relevant lines."""
    keyword1 = "失败"
    keyword2 = "没有"
    extracted_lines = []
    total_failures = 0

    with open(combined_log_path, 'r', encoding='utf-8', errors='replace') as log_file:
        all_lines = log_file.readlines()
        log_output_widget.delete(1.0, tk.END)
        log_output_widget.insert(tk.END, ''.join(all_lines))  # Insert all logs

        for line in all_lines:
            if keyword1 in line or keyword2 in line:
                extracted_lines.append(line.strip())
                total_failures += 1

    failure_count_widget.delete(1.0, tk.END)
    failure_count_widget.insert(tk.END, f"Number of tests failed: {total_failures}")

    failed_tests_widget.delete(1.0, tk.END)
    if extracted_lines:
        failed_tests_widget.insert(tk.END, '\n'.join(extracted_lines))
    else:
        failed_tests_widget.insert(tk.END, "无")

    return extracted_lines, total_failures

def main_function(output_widget):
    hap1 = resource_path("usbInfo-default-signed.hap")
    hap1BundleName = "com.example.nomralapp"
    hap1Ability = "UsbInfoAbility"

    hap2 = resource_path("usb_fullAutomation_newsigned.hap")
    hap2BundleName = "com.example.automationapp"
    hap2Ability = "UsbAutomationAbility"

    template_path = resource_path("allow_button_template.jpeg")

    log1_remote_path = "/data/app/el2/100/base/com.example.nomralapp/haps/usbInfo/files/usb_info.log"
    log2_remote_path = "/data/app/el2/100/base/com.example.automationapp/haps/usbAutomation/files/usb_automation.log"
    combined_log_path = "combined_log.log"

    try:
        install_hap(hap1, output_widget)
        install_hap(hap2, output_widget)

        run_hap(hap1Ability, hap1BundleName, output_widget)
        
        time.sleep(3)

        screenshot_path = capture_screenshot(output_widget)

        x, y = find_button(template_path, screenshot_path)

        tap_screen(x, y, output_widget)

        run_hap(hap2Ability, hap2BundleName, output_widget)

        time.sleep(8)

        retrieve_and_combine_logs(log1_remote_path, log2_remote_path, combined_log_path, output_widget)

        time.sleep(1)
        
        analyze_logs(combined_log_path, log_output_text, failure_count_text, failed_tests_text)
        
    except Exception as e:
        output_widget.insert(tk.END, str(e))
        output_widget.see(tk.END)

def execute_main_function():
    terminal_output_text.delete(1.0, tk.END)
    log_output_text.delete(1.0, tk.END)
    failure_count_text.delete(1.0, tk.END)
    failed_tests_text.delete(1.0, tk.END)
    threading.Thread(target=main_function, args=(terminal_output_text,)).start()

# Create the main window
root = tk.Tk()
root.title("Test Automation GUI")

style = ttk.Style()
style.configure('TButton', font=('Arial', 10))
style.configure('TLabel', font=('Arial', 10))
style.configure('TText', font=('Arial', 10))

# Create the button to execute the main function
execute_button = ttk.Button(root, text="Execute", command=execute_main_function)
execute_button.pack(pady=10)

# Create the output areas
terminal_output_label = ttk.Label(root, text="Terminal Output:")
terminal_output_label.pack()
terminal_output_text = scrolledtext.ScrolledText(root, height=10, font=('Arial', 10))
terminal_output_text.pack(pady=5)

log_output_label = ttk.Label(root, text="Log Output:")
log_output_label.pack()
log_output_text = scrolledtext.ScrolledText(root, height=10, font=('Arial', 10))
log_output_text.pack(pady=5)

failure_count_label = ttk.Label(root, text="Number of Tests Failed:")
failure_count_label.pack()
failure_count_text = scrolledtext.ScrolledText(root, height=2, font=('Arial', 10))
failure_count_text.pack(pady=5)

failed_tests_label = ttk.Label(root, text="Failed Tests:")
failed_tests_label.pack()
failed_tests_text = scrolledtext.ScrolledText(root, height=5, font=('Arial', 10))
failed_tests_text.pack(pady=5)

# Run the GUI event loop
root.mainloop()