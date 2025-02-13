import json
import time
import random
import re
import requests
import logging
import http.client


logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

http.client.HTTPConnection.debuglevel = 1
requests_log = logging.getLogger("requests.packages.urllib3")
requests_log.setLevel(logging.DEBUG)
requests_log.propagate = True

import ddddocr

ocr = ddddocr.DdddOcr()


def index(session):
    rsp = session.get("http://115.28.96.217:8088/zwwsbm/webRegister/index.aspx")
    html = rsp.content.decode('gb2312')
    view_state = re.findall('id="__VIEWSTATE" value="([^"]*)" />', html)[0]
    view_state_generator = re.findall('id="__VIEWSTATEGENERATOR" value="([^"]*)" />', html)[0]
    return view_state, view_state_generator

def captcha_ocr(session):
    while True:
        rsp = session.get('http://115.28.96.217:8088/zwwsbm/webRegister/login/gif.aspx')
        result = ocr.classification(rsp.content).replace('s', '5')
        if re.fullmatch('[0-9]{4}', result):
            return result

def login(session, view_state, view_state_generator, captcha_value):
    data = {
        "__VIEWSTATE": view_state,
        "__VIEWSTATEGENERATOR": view_state_generator,
        "txtSFZH": "612724199608230070",
        "txtMM": "LwuvjOtbSCiPpvTpQVA8mA==",
        "txtYZM": captcha_value
    }

    login_success_body = b"<script>window.location.href='./nMain/frm_main.aspx';</script>"

    rsp = session.post("http://115.28.96.217:8088/zwwsbm/webRegister/index.aspx", data=data)
    if rsp.content.strip() != login_success_body:
        print(rsp.content.decode('gb2312', errors='ignore'))
        raise Exception('login fail')

    session.get('http://115.28.96.217:8088/zwwsbm/webRegister/nMain/frm_main.aspx') # 不加这行的话, 获取数据会返回 "选择考试" 的页面, 猜测: 该页面触发后台获取当前考试

def logout(session):
    return session.get("http://115.28.96.217:8088/zwwsbm/webRegister/exit.aspx").content.decode('gb2312', errors='ignore')

def getInfo(session):
    data= {'postTag': 'getjqgrid', 'hidUnit': '25107', '_search': 'false', 'nd': '1739417064824', 'rows': '1000', 'page': '1', 'sidx': '职位代码', 'sord': 'desc'}
    rsp = session.post("http://115.28.96.217:8088/zwwsbm/webRegister/editInfo/zwDetail.aspx", data=data)
    try:
        body = rsp.content.decode('gb2312', errors='ignore')
        return json.loads(body)
    except Exception:
        logging.error(body)



session = requests.Session()
session.headers = {
    "accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7",
    "accept-language": "zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6",
    "cache-control": "no-cache",
    "pragma": "no-cache",
    "upgrade-insecure-requests": "1",
    "Referer": "http://115.28.96.217:8088/zwwsbm/webRegister/nMain/frm_main.aspx",
    "Referrer-Policy": "strict-origin-when-cross-origin"
}

view_state, view_state_generator = index(session)
logging.info(view_state + ',' +  view_state_generator)

captcha_value = captcha_ocr(session)
logging.info(captcha_value)

login(session, view_state, view_state_generator, captcha_value)

'''
    session.cookies.update({
            ".ASPXCOOKIEAUTH": "8E4EF61C6DF8C7D4C2BC3CAFB57B7AB31DC3D0FC1D87722ABE8B7D646D7B27E97D260EB9E770D5891AC5F6669621F6555F6DE32E332ACBF4B8DEC58266178CD6412CB3C3F4430FE8EC3EBF4088A45819578C1277D72C3377445C18BA6F1FB074A5656DF4D70ED84BF28A65F3383C8378460E3BF612EA433355858CDF1221AA3B05ADB248",
            "ASP.NET_SessionId": "1yo3fbaqldvdkfqnbvele0eu",
            "userexamsort": ""
    })

    cookie_string = "; ".join([f"{cookie.name}={cookie.value}" for cookie in session.cookies])
    print(cookie_string)
'''

try:
    while True:
        info = getInfo(session)
        with open('info.json', 'w') as f:
            f.write(json.dumps(info))
        time.sleep(5 + 5 * random.random()) # sleep 5 ~ 10 seconds
except KeyboardInterrupt:
    print("stopping")
except Exception as exp:
    print(traceback.format_exc())
finally:
    print(logout(session))
