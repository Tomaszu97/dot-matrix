from PIL import Image
import serial
from time import sleep
from numpy.random import choice
# Keep in mind image has to be
# 16px wide
# Height is n*8
# Regular BMP color format
# #FFFFFF pixels will lit up
# Any other color is dark

serial_conn = serial.Serial(
    port='COM3',
    baudrate=38400,
    timeout=2,
    parity=serial.PARITY_NONE,
    rtscts=0,
)
sleep(2)


def load_anim(filename):
    img = Image.open(filename)
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
    return animation_data


def send_anim(anim):
    serial_conn.write(anim.encode('ASCII'))


def set_fps(fps):
    serial_conn.write(f'FPS:{fps}#'.encode('ASCII'))


def set_brightness(brightness):
    serial_conn.write(f'BRIGHTNESS:{brightness}#'.encode('ASCII'))


animations = [
    'blink.bmp',
    'eyes.bmp',
    'lookaround.bmp',
    'sine.bmp',
    'sine-inverted.bmp',
]
weights = [0.3, 0.5, 0.15, 0.03, 0.02]
fps = [6, 1, 3, 24, 24]

last_anim_name = ''
while(1):
    anim_name = choice(animations, p=weights)
    if anim_name != last_anim_name:
        last_anim_name = anim_name
        anim = load_anim(anim_name)
        send_anim(anim)
        set_brightness(4)
        idx = [idx for idx, a in enumerate(animations) if a == anim_name][0]
        set_fps(fps[idx])

    sleep(5)

serial_conn.close()
