from Cryptodome.Hash import SHA256
from Cryptodome.PublicKey import RSA
from Cryptodome.Signature import pkcs1_15
import os

if __name__ == '__main__':
	filename = input("input document filename: ")
	action = input("input action (sign or check):")
	if action == 'sign':
		try:
			print("getting hash of the message...")
			h = SHA256.new()
			with open(filename, "rb") as f:
				for chunk in iter(lambda: f.read(4096), b""):
					h.update(chunk)

			print("creating public and private keys...")
			pubkeyfilename = input("where to save public key (" + filename + ".key.pub by default): ")
			if (pubkeyfilename == ""):
				pubkeyfilename = filename + ".key.pub"
			key = RSA.generate(1024, os.urandom)
			print("saving public key to ", pubkeyfilename , "...")
			with open(pubkeyfilename, "wb") as pubkeyfile:
				pubkeyfile.write(key.publickey().export_key('PEM'))
			print("keys are created")

			print("creating signature...")
			signaturefilename = input("where to save signature (" + filename + ".signature by default): ")
			if (signaturefilename == ""):
				signaturefilename = filename + ".signature"
			signature = pkcs1_15.new(key).sign(h)
			with open(signaturefilename, "wb") as signfile:
				signfile.write(signature)
			print("signed successfully")

		except(FileNotFoundError):
			print("file is not found")
			exit(0)

	elif action == 'check':
		try:
			print("getting hash of the message...")
			h = SHA256.new()
			with open(filename, "rb") as f:
				for chunk in iter(lambda: f.read(4096), b""):
					h.update(chunk)

			pubkeyfilename = input("input public key filename: ")
			with open(pubkeyfilename, "rb") as pubkeyfile:
				publickey = RSA.import_key(pubkeyfile.read()).publickey()
			print("key is read")

			signaturefilename = input("input signature filename: ")
			with open(signaturefilename, "rb") as signaturefile:
				signature = signaturefile.read()
			print("signature is read")

			pkcs1_15.new(publickey).verify(h, signature)
			print("signature is confirmed")

		except(FileNotFoundError):
			print("file not found")
			exit(0)
		except(ValueError):
			print("Signature is not confirmed")
			exit(0)

	else:
		print("invalid action")
