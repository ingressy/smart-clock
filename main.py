import digitalio, board, os, openmeteo_requests, requests_cache, requests, time
from lib import epd2in13_V4
from PIL import Image, ImageDraw, ImageFont
from retry_requests import retry

src_dir = 'src'

mled = digitalio.DigitalInOut(board.D5)
mled.direction = digitalio.Direction.OUTPUT

def setup():
    try:
        global w
        global h
        global display
        global body
        global responses
        global pegelonline_url

        display = epd2in13_V4.EPD()
        display.init()
        display.Clear(255)

        w = display.width
        h = display.height

        body = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=18)

    except IOError as e:
        print(e)
        display.sleep()

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

    pegelonline_url = requests.get("https://www.pegelonline.wsv.de/webservices/rest-api/v2/stations/c1f059dd-5063-468a-9dfa-0360fe0b030d/W/currentmeasurement.json")

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
    time.sleep(15)

def dashboard():
    display.init()
    display.Clear(255)
    mled.value = False

    image = Image.new(mode='1', size=(h, w), color=255)
    draw = ImageDraw.Draw(image)

    response = responses[0]
    current = response.Current()
    temp = current.Variables(0).Value()
    wind_speed = current.Variables(1).Value()
    wind_direc = current.Variables(2).Value()
    wind_gusts = current.Variables(3).Value()

    temp_ro = round(temp, 2)
    windspeed_ro = round(wind_speed, 2)
    windgusts_ro = round(wind_gusts,2)
    winddirec_ro = round(wind_direc, 2)

    wasserstand = pegelonline_url.json()

    ltime = time.localtime()
    fortime = time.strftime("%H:%M", ltime)

    dash = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=12)
    time_font = ImageFont.truetype(font=os.path.join(src_dir, 'Roboto-Regular.ttf'), size=28)
    draw.text((0,0), f"{temp_ro}°C  {windspeed_ro}kn/{windgusts_ro}kn  {winddirec_ro}°  {wasserstand.get('value')}cm", font=dash, fill=0, align='left')
    draw.text((0,15), f"{fortime}",font=time_font, fill=0, align='left')
    display.display(display.getbuffer(image))

    display.sleep()
    time.sleep(30)

def main():
    start()
    dashboard()

if __name__ == "__main__":
    main()
