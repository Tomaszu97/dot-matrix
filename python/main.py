from PIL import Image
import serial
from time import sleep
# Keep in mind image has to be
# 16px wide
# Height is n*8
# Regular BMP color format
# #FFFFFF pixels will lit up
# Any other color is dark


img = Image.open('anim1.bmp')
pixels = img.load()
animation_data = 'DATA:\n'
for y in range(img.height):
    for x in range(img.width):
        if(pixels[x, y] == (255, 255, 255)):
            animation_data += '1'
        else:
            animation_data += '0'
    animation_data += '\n'
animation_data += '#'

serial_conn = serial.Serial(
    port='COM3',
    baudrate=38400,
    timeout=2,
    parity=serial.PARITY_NONE,
    rtscts=0,
)

sleep(2)

serial_conn.write(animation_data.encode('ASCII'))

serial_conn.write('BRIGHTNESS:7#'.encode('ASCII'))

serial_conn.write('FPS:6#'.encode('ASCII'))

serial_conn.close()

print(animation_data)
