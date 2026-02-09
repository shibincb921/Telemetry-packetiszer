import serial
HEADER = [0xFD, 0xFC, 0xFB, 0xFA]
TAIL   = [0x04, 0x03, 0x02, 0x01]
TOTAL_LEN = 12

ser = serial.Serial(
    port='COM5',        # according the port
    baudrate=115200,
    timeout=0.1
)

def read_packet():
    while True:
        b = ser.read(1)
        if not b:
            return None

        
        if b[0] != HEADER[0]:
            continue

       
        hdr = ser.read(3)
        if len(hdr) != 3:
            continue

        if [b[0], hdr[0], hdr[1], hdr[2]] != HEADER:
            continue

        
        rest = ser.read(TOTAL_LEN - 4)
        if len(rest) != TOTAL_LEN - 4:
            continue

        packet = bytes(HEADER) + rest

        
        length = packet[4]
        if length != 2:
            continue

     
        calc_chk = xor_checksum(packet[0:7])
        if calc_chk != packet[7]:
            continue

       
        if list(packet[8:12]) != TAIL:
            continue

        return packet
def xor_checksum(data):
    chk = 0
    for b in data:
        chk ^= b
    return chk
while True:
    pkt = read_packet()
    if pkt:

        payload0 = pkt[5]
        payload1 = pkt[6]

        #value = (payload0 << 8) | payload1
        voltage=(payload0<<8) | (payload1>>4)
        volatge=voltage/100
        button_sts1=payload1&0x01
        button_sts2=(payload1&0x02)>>1
        button_sts3=(payload1&0x04)>>2
        button_sts4=(payload1&0x08)>>3
        print("voltage:", voltage)
        print("|Buttons:[",button_sts1,",",button_sts2,",",button_sts3,",",button_sts4,"]\n")
