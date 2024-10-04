from os import getenv

import digitalio, board, os, openmeteo_requests, requests_cache, time, datetime, webuntis.objects
from lib import epd2in13_V4
from PIL import Image, ImageDraw, ImageFont
from retry_requests import retry
from time import sleep

src_dir = 'src'

mled = digitalio.DigitalInOut(board.D5)
mled.direction = digitalio.Direction.OUTPUT

def setup():
    global w
    global h
    global display
    global body

    try:
        display = epd2in13_V4.EPD()
        display.init()
        display.Clear(255)

        w = display.width
        h = display.height

        body = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=18)

    except IOError as e:
        print(e)
        display.sleep()

def start():
    mled.value = True
    setup()

    title = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=28)
    boot = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=18)
    image = Image.new(mode='1', size=(h, w), color=255)
    draw = ImageDraw.Draw(image)
    draw.text((0, 30), 'm~cat v0.4 DEV', font=title, fill=0, align='left')
    draw.text((0, 55), 'booting ...', font=boot, fill=0, align='left')
    display.display(display.getbuffer(image))
    display.sleep()

    global responses
    global s


    cache_session = requests_cache.CachedSession('.cache', expire_after=3600)
    retry_session = retry(cache_session, retries=5, backoff_factor= 0.2)
    openmeteo = openmeteo_requests.Client(session=retry_session)

    url = "https://api.open-meteo.com/v1/forecast"
    params = {
        "latitude": 53.0451,
        "longitude": 8.8535,
        "current": ["temperature_2m", "wind_speed_10m", "wind_direction_10m", "wind_gusts_10m"],
        "wind_speed_unit": "kn"
    }
    responses = openmeteo.weather_api(url, params=params)


    time.sleep(15)

def overlay():
    display.init()
    display.Clear(255)

    image = Image.new(mode='1', size=(h, w), color=255)
    draw = ImageDraw.Draw(image)

    response = responses[0]
    current = response.Current()
    temp = current.Variables(0).Value()
    wind_speed = current.Variables(1).Value()
    wind_direc = current.Variables(2).Value()
    wind_gusts = current.Variables(3).Value()

    temp_ro = round(temp, 1)
    windspeed_ro = round(wind_speed, 2)
    windgusts_ro = round(wind_gusts,2)
    winddirec_ro = round(wind_direc, 2)

    ltime = time.localtime()
    fortime = time.strftime("%H:%M:%S", ltime)

    dash = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=12)
    draw.text((0,0), f"{temp_ro}°C  {windspeed_ro}kn/{windgusts_ro}kn  {winddirec_ro}°", font=dash, fill=0, align='left')
    draw.text((0, 110), f"last update: {fortime}", font=dash, fill=0, align='left')
    display.display(display.getbuffer(image))

    display.sleep()

def weather():
    global activ_site
    display.init()
    image = Image.new(mode='1', size=(h, w), color=255)
    draw = ImageDraw.Draw(image)

    response = responses[0]
    current = response.Current()
    temp = current.Variables(0).Value()
    wind_speed = current.Variables(1).Value()
    wind_direc = current.Variables(2).Value()
    wind_gusts = current.Variables(3).Value()

    temp_ro = round(temp, 1)
    windspeed_ro = round(wind_speed, 2)
    windgusts_ro = round(wind_gusts, 2)
    winddirec_ro = round(wind_direc, 2)

    ltime = time.localtime()
    fortime = time.strftime("%H:%M:%S", ltime)

    text_font = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=14)
    update_font = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=12)
    bold = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Bold.ttf'), size=14)

    draw.text((0, 10), f"Zimmertemperatur: 25°C", font=text_font, fill=0, align='left')
    draw.text((0, 30), f"Daten für Bremen, Habenhausen:", font=bold, fill=0, align='left')
    draw.text((0, 42), f"Temperatur: {temp_ro}°C", font=text_font, fill=0, align='left')
    draw.text((0, 54), f"Windgeschwindigkeit: {windspeed_ro}kn", font=text_font, fill=0, align='left')
    draw.text((0, 66), f"Böengeschwindigkeit: {windgusts_ro}kn", font=text_font, fill=0, align='left')
    draw.text((0, 78), f"Windrichtung: {winddirec_ro}°", font=text_font, fill=0, align='left')

    draw.text((0, 110), f"last update: {fortime}", font=update_font, fill=0, align='left')

    display.display(display.getbuffer(image))
    display.sleep()
    activ_site = 2

def overlay_update():
    global activ_site
    display.init()
    image = Image.new(mode='1', size=(h, w), color=255)
    draw = ImageDraw.Draw(image)

    response = responses[0]
    current = response.Current()
    temp = current.Variables(0).Value()
    wind_speed = current.Variables(1).Value()
    wind_direc = current.Variables(2).Value()
    wind_gusts = current.Variables(3).Value()

    temp_ro = round(temp, 1)
    windspeed_ro = round(wind_speed, 2)
    windgusts_ro = round(wind_gusts, 2)
    winddirec_ro = round(wind_direc, 2)

    ltime = time.localtime()
    fortime = time.strftime("%H:%M:%S", ltime)

    dash = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=12)
    draw.text((0, 0), f"{temp_ro}°C  {windspeed_ro}kn/{windgusts_ro}kn  {winddirec_ro}°", font=dash, fill=0, align='left')
    draw.text((0, 110), f"last update: {fortime}", font=dash, fill=0, align='left')
    display.displayPartial(display.getbuffer(image))

    display.sleep()
    activ_site = 1

def update_routine():
    while True:
        if activ_site == "1":
            sleep(300)
            overlay_update()
        if activ_site == "2":
            sleep(1800)
            weather()

def main():
    start()
    mled.value = False
    weather()
    update_routine()


if __name__ == "__main__":
    main()
