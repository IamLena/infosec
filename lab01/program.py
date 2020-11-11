from sys import platform
from subprocess import check_output
import hashlib
import os

def get_CPUsum():
	if platform == "win32":  # wmic csproduct get  UUID
		output = check_output("wmic csproduct get UUID", shell=True).decode()
		hard_uuid = output.split("\n")[1]
		output = check_output("wmic csproduct get IdentifyingNumber", shell=True).decode()
		serial_num = output.split("\n")[1]
	check_str = hard_uuid + " " + serial_num
	return hashlib.sha256(check_str.encode('utf-8')).hexdigest()

def check_CPUsum():
	real_key = from_license_key()
	if real_key and real_key == get_CPUsum():
		return True
	else:
		return False

def from_license_key():
	try:
		with open("license.key", "r") as lic_file:
			return lic_file.readline()
	except FileNotFoundError:
		return

if __name__ == "__main__":
	is_have_license = check_CPUsum()

	if is_have_license:
		print("You have a license!")
	else:
		print("Sorry, access is denied!")
