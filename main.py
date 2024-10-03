from time import sleep
import digitalio, board, os
from lib import epd2in13_V4
from PIL import Image, ImageDraw, ImageFont

src_dir = 'src'

mled = digitalio.DigitalInOut(board.D5)
mled.direction = digitalio.Direction.OUTPUT

def setup():
    try:
        global w
        global h
        global display
        global body

        display = epd2in13_V4.EPD()
        display.init()
        display.Clear(0)

        w = display.width
        h = display.height
        print('width:', h)
        print('height:', w)

        body = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=18)

    except IOError as e:
        print(e)
        display.sleep()

def start():
    setup()

    body = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=18)
    image = Image.new(mode='1', size=(h, w), color=255)
    draw = ImageDraw.Draw(image)
    draw.text((0, 0), 'm~cat v0.4 DEV', font=body, fill=0, align='left')
    draw.text((0, 15), 'booting ...', font=body, fill=0, align='left')
    display.display(display.getbuffer(image))
    display.sleep()
    sleep(20)

    mled.value = True

def dashboard():
    mled.value = False

def main():
    start()
    dashboard()

if __name__ == "__main__":
    main()
