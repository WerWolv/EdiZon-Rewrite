from ftplib import FTP
import sys
import os

crashLog = []

def writeFunc(s):
    global crashLog
    crashLog += str(s, 'utf-8').split('\n')

ftp = FTP()
ftp.connect(sys.argv[2], int(sys.argv[3])) 
ftp.login()
ftp.cwd("/atmosphere/crash_reports")

filelist = ftp.nlst()
ftp.retrbinary(f"RETR {filelist[len(filelist) - 1]}", writeFunc)

os.system(f"aarch64-none-elf-addr2line -f -p -C -e {sys.argv[1]} -a {str(crashLog[49][63:-1])}")

i = 0
while not crashLog[51 + i].endswith("0000000000000000"):
    os.system(f"aarch64-none-elf-addr2line -f -p -C -e {sys.argv[1]} -a {str(crashLog[51 + i][63:-1])}")
    i = i + 1