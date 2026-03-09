#The following is ChatGPT output with the prompt:
# “given this document, give me a micropy code to interface with the fingerprint sensor.”
# The pdf at this link was also attatched: https://m5stack-doc.oss-cn-shenzhen.aliyuncs.com/1186/Unit-Fingerprint-Protocol-EN-V1.0.pdf

from machine import UART
import time
import struct

class UnitFingerprint:
    HEADER = b'\xEF\x01'
    ADDRESS = b'\xFF\xFF\xFF\xFF'

    PKT_CMD = 0x01
    PKT_ACK = 0x07

    # Common commands
    CMD_WORK_MODE = 0xD3
    CMD_WAKE = 0xD4
    CMD_GET_ENROLL_IMG = 0x29
    CMD_GEN_CHAR = 0x02
    CMD_GET_IMG = 0x01
    CMD_MATCH = 0x03
    CMD_SRCH = 0x04
    CMD_STORE_CHAR = 0x06
    CMD_SET_WRK_MODE = 0xD2
    CMD_EMPTY = 0x0D
    CMD_REG_MODEL = 0x05
    
    
    

    def __init__(self, uart_id=1, tx=17, rx=16):
        self.uart = UART(
            uart_id,
            baudrate=115200,
            bits=8,
            parity=None,
            stop=1,
            tx=tx,
            rx=rx,
            timeout=1000
        )

    # ---------------- low level ----------------

    def _checksum(self, data):
        return sum(data) & 0xFFFF

    def _send_packet(self, pkt_type, payload):
        length = len(payload) + 2
        pkt = bytearray()
        pkt += self.HEADER
        pkt += self.ADDRESS
        pkt.append(pkt_type)
        pkt += struct.pack(">H", length) 
        pkt += payload


        cs = self._checksum(pkt[6:])   # from identifier onward
        pkt += struct.pack(">H", cs)

        self.uart.write(pkt)
        print(f"Sending: {pkt.hex()}")
        
    def read_exact(self, n):
        buf = bytearray()
        timeout = 10
        i = 0
        while len(buf) < n:
            chunk = self.uart.read(n - len(buf))
            if chunk:
                buf.extend(chunk)
            if i >= timeout:
                print("timeout")
                return None
            i += 1
        return bytes(buf)
  
    def _get_packet(self):
        header = b'\xEF\x01\xFF\xFF\xFF\xFF'
        start = self.read_exact(9) #read 9 bytes
        
        if start is None or start[0:6] != header: #Ensure the header is correct
            return None
        
        length = start[7:]
        
        payload = self.read_exact(int.from_bytes(length, "big")) #get payload
        
        #TODO: do checksum
        pkt = start + payload
        return pkt
        
        
    def _read_packet(self):
        # basic blocking read
        head = self._get_packet()
        if head:
            #print(f"Received: {head.hex()}")
            pass
        else:
            return None
        
        length = struct.unpack(">H", head[7:9])[0]
        body = head[9:]
        ident = head[6]
        code = body[0]
        params = None
        if(length > 3): #if length > 3, it has parameters   
            params = body[1:-2]

        return ident, code, params

    def _command(self, cmd, params=b''):
        payload = bytes([cmd]) + params
        self._send_packet(self.PKT_CMD, payload)
        return self._read_packet()

    # ---------------- high level API ----------------
    
    def wake(self):
        return self._command(self.CMD_WAKE)
    
    def get_work_mode(self):
        return self._command(self.CMD_WORK_MODE)
    def set_work_mode(self, mode):
        return self._command(self.CMD_SET_WRK_MODE, bytes([mode]))
    
    def get_enroll_img(self):
       return self._command(self.CMD_GET_ENROLL_IMG)
    
    def gen_char(self, count):
        return self._command(self.CMD_GEN_CHAR, bytes([count]))
    
    def get_img(self):
        return self._command(self.CMD_GET_IMG)
    
    def match(self):
        return self._command(self.CMD_MATCH)
    
    def search(self):
        return self._command(self.CMD_SEARCH)
    
    def reg_model(self):
        return self._command(self.CMD_REG_MODEL)
    
    def store(self, buffID, pos):
        return self._command(self.CMD_STORE_CHAR, bytes([buffID, pos]))
    
    def empty(self):
        return self._command(self.CMD_EMPTY)

    def timer(self, t):
        #Wait t number of seconds and print out how many seconds
        while t > 0:
            print(f"{t}... ",end="")
            time.sleep(1)
            t -= 1
        print("")
    def enroll(self, maxCount=2):
        #enrolls fingerprint
        self.wake()
        self.set_work_mode(1)
        #print("Work mode:", self.get_work_mode()) #For testing
        print("Empty: ", self.empty())
        
        count = 1
        while count <= maxCount:
            if count == 1:
                print("Place finger...")
                self.timer(3)
            else:
                print("Remove and replace finger...")
                self.timer(3)
    
            print("Enrolled image: ", self.get_enroll_img())
            print("Gen char: ", self.gen_char(count))
            
        print("Reg Model: ", self.reg_model())
        print("Store: ", hex(self.store(1, 1)[1]))
            count += 1
        self.set_work_mode(0)
            
    def check_match(self):
        #Checks finger if it matches
        self.wake()
        self.set_work_mode(1)
        self.get_img()
        print("Match: ", hex(self.match()[1]))
        self.set_work_mode(0)
        

if __name__ == "__main__":
    fp = UnitFingerprint(uart_id=1, tx=25, rx=34)
    
    fp.enroll()
    
    fp.check_match()
    
