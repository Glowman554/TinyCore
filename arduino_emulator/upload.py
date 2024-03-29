import serial
import sys
import tqdm

class Tunnel:
	def __init__(self, port):
		self.port = serial.Serial(port, 9600, timeout=1)
		print("Connecting...")
		self.port.read_until(b'READY\n')
		self.command("PING")

	def _recv(self):
		tmp = self.port.readline()
		print("<-", tmp)
		return tmp

	def _send(self, b):
		print("->", b)
		self.port.write(b)

	def command(self, command):
		self._send(str.encode(command + "\n"))
		tmp = self._recv().decode()
		if tmp == "":
			raise Exception("Command failed")
		return tmp


tunnel = Tunnel(sys.argv[1])


addr = 0
with open(sys.argv[2]) as f:
	for line in f:
		line = line.strip()
		if line[0] == '@':
			addr = int(line[1:], 16)
		else:
			tunnel.command(f"WRITE {addr} {int(line, 16)}")
			addr += 1
