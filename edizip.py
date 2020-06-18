# LyfeOnEdge March 17 2020 GPL2
# www.brewtools.dev (Lyfe's Discord and homebrew repository site)
# Python3 module / script for generating .edz style archives.
import os, sys, time, struct, zipfile, random
from uuid import UUID
from io import BytesIO

DEFAULT_TID = 0
MAGIC = 0x4E5A4445 #EDZN
u64_MAX = 0xFFFFFFFFFFFFFFFF
HEADER_LENGTH = 37

# Original zipit code:
# http://stackoverflow.com/a/6078528
#
# Call `zipit` with the path to either a directory or a file.
# All paths packed into the zip are relative to the directory
# or the directory of the file.

# zipit has been modified to not require an archive name (since it's not writing to one)
# it now uses a bytes object to store the zip contents so it never hits disk
def zipit(path):
	virtual_zip = BytesIO()
	archive = zipfile.ZipFile(virtual_zip, "w", zipfile.ZIP_DEFLATED)
	if os.path.isdir(path):
		_zippy(path, path, archive)
	else:
		_, name = os.path.split(path)
		archive.write(path, name)
	return virtual_zip
	
def _zippy(base_path, path, archive):
	paths = os.listdir(path)
	print(paths)
	for p in paths:
		p = os.path.join(path, p)
		if os.path.isdir(p):
			_zippy(base_path, p, archive)
		else:
			archive.write(p, os.path.relpath(p, base_path))

class Edizip:
	"""Object to handle zipping edizip files"""
	def __init__(self, magic = MAGIC, tid = DEFAULT_TID):
		self.magic = magic
		self.tid = tid

	def set_magic(self, magic: int):
		"""Change magic"""
		self.magic = magic

	def set_tid(self, tid: int):
		"""Set tid"""
		self.tid = int(int)

	def generate_random_UID(self):
		"""Generate a random UID for the header"""
		UID = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
		print(f"Generated random UID {UID}")
		return UUID(int=UID)

	def make_header(self, TID, UID: int, DELTA: bool = False):
		"""returns an edz header in bytes object form"""
		if not TID: #If not passed, use default
			TID = self.tid
		t = int(time.time()) #Get current time since epoch in decimal
		magic = struct.pack("<I", int(self.magic))
		print(f"Making header: Magic - {magic}, TID - {TID}, UID - {UID}, Delta - {DELTA}, Timestamp - {t}")
		packed_header = struct.pack("<IQQQQ?",
			int(self.magic), #u32 - I
			int(TID), #u64 - Q
			int((UID.int >> 64) & u64_MAX), #u128 1/2 - Q
			int(UID.int & u64_MAX), #u128 2/2 - Q
			int(t),  #u64 - Q
			DELTA # bit - ?
		)
		print(f"Header: {packed_header}")
		return packed_header

	def check_magic(self, target: str):
		"""Returns true if file header magic matches"""
		with open(target, "rb+") as archive:
			magic = archive.read(4)
			if magic == struct.pack("I", self.magic):
				return True
			else:
				return False

	def unpack_header(self, header):
		magic, TID, UIDA, UIDB, t, DELTA = struct.unpack("<IQQQQ?",header)
		mgc = struct.pack("<I", magic)
		UID = hex((UIDA << 64) | UIDB)
		print(f"Unpacked header: Magic - {mgc}, TID - {TID}, UID - {UID}, Delta - {DELTA}, Timestamp - {t}")
		return (magic, TID, UID, t, DELTA)

	def get_header(self, target: str):
		"""Returns the target file's header, False if invalid magic, None on Error"""
		try:
			if not self.check_magic(target):
				print("File magic invalid.")
				return False
			with open(target, "rb+") as archive:
				header = archive.read(HEADER_LENGTH)
				return self.unpack_header(header)
		except Exception as e:
			print(f"Failed to get header for {target} - {e}")
			return None

	def archive(self, header, target: str, output_target: str = None):
		"""Function to make edizip, returns edz location if sucessful"""
		try:
			target = os.path.abspath(target)
			if output_target:
				outfile = output_target
			else:
				if os.path.isfile(target):
					outfile = target + ".edz"
				elif os.path.isdir(target):
					outfile = os.path.join(target, os.path.basename(target) + ".edz")
				else:
					print("No valid output target")
					return
			#Zip target dir
			print(f"Creating virtual zip of {target}")
			zip_bytes_object = zipit(target)
			print(f"Making .edz at {outfile}...")
			with open(outfile, "w+b") as out:
				print("Writing header...")
				out.write(header)
				print("Writing zip contents...")
				out.write(zip_bytes_object.getvalue())
			print("Success!")
			return outfile
		except Exception as e:
			print(f"Failed to create edizip from target {target} - {e}")
			print("Attempting cleanup...")
			try:
				if os.path.isfile(outfile):
					print(f"Removing possibly invalid archive {outfile}")
					os.remove(outfile)
			except:
				pass

	def unarchive(self, target: str, output_target: str = None):
		"""Function to decompress edizip, returns a tuple containing the status and the header respsectively"""
		"""An unsucessful decompression will result in a False status, and the header will be None"""
		try:
			target = os.path.abspath(target)
			print(f"Decompressing archive {target}")
			if output_target:
				print(f"Using specified output dir {output_target}")
				outdir = output_target
			else:
				outdir = os.path.dirname(target)
				print(f"Decompressing archive in place at {outdir}")

			print("Opening archive...")
			with open(target, "rb+") as archive:
				print(f"Reading header...")
				header = archive.read(HEADER_LENGTH)
				print(f"Reading archive contents...")
				zip_contents = BytesIO(archive.read())
			"Loading archive..."
			zip = zipfile.ZipFile(zip_contents, "r", zipfile.ZIP_DEFLATED)
			print("Extracting...")
			zip.extractall(outdir)
			print("Sucess!")
			status = True

		except Exception as e:
			print(f"Error decompressing archive {target} - {e}")
			status = False
			header = None

		return (status, header)

	def peek(self, target: str):
		"""Returns tuple containing header and file list or None if unsucessful"""
		try:
			target = os.path.abspath(target)
			if output_target:
				outdir = output_target
			else:
				outdir = os.path.dirname(target)

			with open(target, "rb+") as archive:
				header = archive.read(HEADER_LENGTH)
				zip_contents = BytesIO(archive.read())

			zip = zipfile.ZipFile(zip_contents, "r", zipfile.ZIP_DEFLATED)
			status = zip.namelist()
			
		except Exception as e:
			print(f"Error peeking at archive {target} - {e}")
			status = []
			header = None

		return (status, header)

if __name__ == "__main__":
	import argparse
	parser = argparse.ArgumentParser()
	parser.add_argument("target", help = "Target dir to archive")
	parser.add_argument("-o", "--output", help = "Edizip output location, defaults to `target/target.edz` if not set")
	parser.add_argument("-t", "--title", help = "TitleID, defaults to 0")
	parser.add_argument("-d", "--decompress", action='store_true', help = "Decompress file, target becomes archive and output becomes the target output directory. Will decompress in parent dir of archive if output dir not specified.")
	parser.add_argument("-v", "--verify", action='store_true', help = "Verifies file magic is accurate, pass an integer representation of your magic to --magic if checking a file with non-standard magic")
	parser.add_argument("-m", "--magic", help = "Magic to use when generating or checking headers, defaults to b'EDZN'")
	parser.add_argument("-x", "--header", action='store_true', help = "Print file header, will fail if magic does not match.")
	
	args = parser.parse_args()

	zipper = Edizip()

	if args.title:
		zipper.set_tid(int(args.title))

	if args.magic:
		zipper.set_magic(int(args.magic))

	if args.verify:
		"""Check if valid magic"""
		print("Checking magic...")
		if zipper.check_magic(args.target):
			print("Valid magic.")
		else:
			print("Invalid magic.")
	elif args.header:
		zipper.get_header(args.target)
	elif args.decompress:
		"""Decompress archive"""
		zipper.unarchive(args.target, args.output)
	else:
		"""Compress archive"""
		uid = zipper.generate_random_UID()
		header = zipper.make_header(None, uid)
		zipper.archive(header, args.target, args.output)