from subprocess import *
import os
import sys

http = 'http://'
https = 'https://'
FNULL=open(os.devnull, 'w')
web_list = open('weblist_all', 'r').read().split('\n')
while web_list[-1] == "":
    del web_list[-1]

mmwebreplay = os.path.join(os.environ['mmpath'], 'usr/bin/mm-webreplay')
mmlink = os.path.join(os.environ['mmpath'], 'usr/bin/mm-link')
mmdelay = os.path.join(os.environ['mmpath'], 'usr/bin/mm-delay')
repo =  os.path.join(os.environ['mmpath'], 'tmp')

i = 0
for web in web_list[:50]:
    web = web.split(',')
    i += 1
    sys.stdout.flush()
    aurl = https + web[1] if web[2] == 'True' else http + web[1]
    url = http + web[1]
    print(str(i) + " replay: " + aurl)
    web = web[1]
    try:
        call([mmwebreplay, os.path.join(repo, web), mmdelay, '0', os.path.join(repo, web), '--', 'python3', 'chrome.py', aurl, "replay"], timeout=60, env=os.environ.copy())
    except:
        call(['pkill', 'chromium'])
