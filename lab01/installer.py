# file == bd os.tmpfile()
from sys import platform
from subprocess import check_output
import hashlib
import os


def get_CPUsum():
	print("platform: ", platform)
	if platform == "win32":  # wmic csproduct get  UUID
		output = check_output("wmic csproduct get UUID", shell=True).decode()
		hard_uuid = output.split("\n")[1]
		output = check_output("wmic csproduct get IdentifyingNumber", shell=True).decode()
		serial_num = output.split("\n")[1]
	check_str = hard_uuid + " " + serial_num
	return hashlib.sha256(check_str.encode('utf-8')).hexdigest()

def check_CPUsum(given_checksum):
	real_key = from_license_key()
	if real_key != get_CPUsum():
		return False
	else:
		return True

def to_license_key(checksum):
	with open("license.key", "w") as lic_file:
		lic_file.write(checksum)

def from_license_key():
	with open("license.key", "r") as lic_file:
		return lic_file.readline()

if __name__ == "__main__":
	to_license_key(get_CPUsum())
	print(from_license_key())
